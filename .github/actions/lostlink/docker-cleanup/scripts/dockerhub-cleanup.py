#!/usr/bin/env python3
"""
Docker Hub Cleanup Script for GitHub Action
Removes old PR and SHA tags from Docker Hub repositories
Uses Docker Registry v2 authentication (works with both passwords and PATs)
"""

import os
import sys
import json
import requests
from datetime import datetime, timedelta, timezone
import re
import argparse
import time
from urllib.parse import quote
from functools import wraps
import base64


class DockerHubCleaner:
    def __init__(self, username, password, dry_run=False, verbose=False, protected_tags=None, custom_patterns=None):
        self.username = username
        self.password = password  # Can be password or Personal Access Token
        self.dry_run = dry_run
        self.verbose = verbose
        self.protected_tags = protected_tags or []
        self.custom_patterns = custom_patterns or {}
        self.hub_url = "https://hub.docker.com/v2"
        self.auth_url = "https://auth.docker.io"
        self.registry_url = "https://registry-1.docker.io/v2"
        self.tokens = {}  # Cache tokens per repository
        self.request_timeout = 30  # 30 seconds timeout for API requests
        self.max_retries = 3
        self.retry_delay = 1  # Initial delay in seconds

        # Track statistics for output
        self.stats = {
            "identified_count": 0,
            "deleted_count": 0,
            "protected_count": 0,
            "repositories": []
        }

    def log(self, message, level="INFO"):
        """Log message with timestamp"""
        timestamp = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC")
        if level == "DEBUG" and not self.verbose:
            return
        # Always output to stderr to keep stdout clean for JSON output
        print(f"[{timestamp}] {message}", file=sys.stderr)

    def retry_with_backoff(func):
        """Decorator for retrying API calls with exponential backoff"""
        @wraps(func)
        def wrapper(self, *args, **kwargs):
            last_exception = None
            delay = self.retry_delay

            for attempt in range(self.max_retries):
                try:
                    return func(self, *args, **kwargs)

                except requests.exceptions.Timeout as e:
                    last_exception = e
                    self.log(f"⏱️  Request timeout (attempt {attempt + 1}/{self.max_retries})", "WARNING")

                except requests.exceptions.HTTPError as e:
                    if e.response.status_code == 429:  # Rate limited
                        retry_after = int(e.response.headers.get('Retry-After', delay))
                        self.log(f"⚠️  Rate limited, waiting {retry_after} seconds...", "WARNING")
                        time.sleep(retry_after)
                        continue
                    elif e.response.status_code == 401:  # Unauthorized
                        self.log(f"  Authentication issue (401): {e}", "DEBUG")
                        # Don't retry auth errors
                        raise e
                    last_exception = e

                except requests.exceptions.RequestException as e:
                    last_exception = e
                    self.log(f"⚠️  Request failed (attempt {attempt + 1}/{self.max_retries}): {e}", "WARNING")

                if attempt < self.max_retries - 1:
                    self.log(f"⏳ Waiting {delay} seconds before retry...", "DEBUG")
                    time.sleep(delay)
                    delay *= 2  # Exponential backoff

            # All retries exhausted
            raise last_exception if last_exception else Exception("Max retries exceeded")

        return wrapper

    def parse_repository_spec(self, repo_spec):
        """
        Parse repository specification to extract namespace and repository name.
        Handles both 'repo' and 'namespace/repo' formats.
        """
        parts = repo_spec.split('/')
        if len(parts) == 2:
            # Format: namespace/repository
            return parts[0], parts[1]
        elif len(parts) == 1:
            # Format: repository (namespace must be provided separately)
            return None, parts[0]
        else:
            raise ValueError(f"Invalid repository format: {repo_spec}. Use 'repository' or 'namespace/repository'")

    def get_basic_auth_header(self):
        """Get basic auth header for authentication"""
        credentials = f"{self.username}:{self.password}"
        encoded = base64.b64encode(credentials.encode()).decode('ascii')
        return f"Basic {encoded}"

    @retry_with_backoff
    def get_bearer_token(self, namespace, repository):
        """Get bearer token for specific repository operations"""
        # Check if we have a cached token for this repository
        cache_key = f"{namespace}/{repository}"
        if cache_key in self.tokens:
            token_data = self.tokens[cache_key]
            # Simple cache - tokens typically last 5 minutes
            if (datetime.now(timezone.utc) - token_data['created']).total_seconds() < 240:  # 4 minutes
                self.log(f"  Using cached token for {cache_key}", "DEBUG")
                return token_data['token']

        # Request new token
        scope = f"repository:{namespace}/{repository}:pull,push,delete"
        url = f"{self.auth_url}/token"
        params = {
            "service": "registry.docker.io",
            "scope": scope
        }

        headers = {
            "Authorization": self.get_basic_auth_header()
        }

        self.log(f"  Requesting bearer token for {namespace}/{repository}...", "DEBUG")

        try:
            response = requests.get(url, params=params, headers=headers, timeout=self.request_timeout)
            response.raise_for_status()
            token = response.json().get("token")

            # Cache the token
            self.tokens[cache_key] = {
                'token': token,
                'created': datetime.now(timezone.utc)
            }

            self.log(f"  ✅ Got bearer token for {namespace}/{repository}", "DEBUG")
            return token

        except requests.exceptions.RequestException as e:
            self.log(f"  Failed to get bearer token for {namespace}/{repository}: {e}", "DEBUG")
            raise

    @retry_with_backoff
    def get_tags_registry(self, namespace, repository):
        """Get tags using Docker Registry API (more reliable)"""
        tags = []
        token = self.get_bearer_token(namespace, repository)

        url = f"{self.registry_url}/{namespace}/{repository}/tags/list"
        headers = {
            "Authorization": f"Bearer {token}",
            "Accept": "application/json"
        }

        try:
            response = requests.get(url, headers=headers, timeout=self.request_timeout)
            response.raise_for_status()
            data = response.json()

            tag_names = data.get("tags", [])
            if not tag_names:
                return []

            self.log(f"  Found {len(tag_names)} tags, fetching details...", "DEBUG")

            for tag_name in tag_names:
                # Registry API doesn't provide last_updated, use current time as fallback
                tags.append({
                    "name": tag_name,
                    "last_updated": datetime.now(timezone.utc).isoformat()
                })

            return tags

        except requests.exceptions.RequestException as e:
            self.log(f"  Registry API failed, trying Hub API: {e}", "DEBUG")
            # Fall back to Hub API
            return self.get_tags_hub(namespace, repository)

    @retry_with_backoff
    def get_tags_hub(self, namespace, repository):
        """Get tags using Docker Hub API (fallback)"""
        tags = []
        page = 1
        page_size = 100

        # Docker Hub API doesn't require authentication for public repos
        # But we'll use basic auth if available
        headers = {}
        if self.username and self.password:
            headers["Authorization"] = self.get_basic_auth_header()

        while True:
            url = f"{self.hub_url}/repositories/{namespace}/{repository}/tags"
            params = {
                "page": page,
                "page_size": page_size
            }

            self.log(f"📄 Fetching page {page} of tags for {namespace}/{repository}...", "DEBUG")

            try:
                response = requests.get(
                    url,
                    headers=headers,
                    params=params,
                    timeout=self.request_timeout
                )
                response.raise_for_status()
                data = response.json()

                if "results" not in data:
                    break

                tags.extend(data["results"])
                self.log(f"  Found {len(data['results'])} tags on page {page}", "DEBUG")

                if not data.get("next"):
                    break

                page += 1

            except requests.exceptions.RequestException as e:
                self.log(f"❌ Failed to get tags for {namespace}/{repository}: {e}", "ERROR")
                break

        return tags

    def get_tags(self, namespace, repository):
        """Get all tags for a repository (tries both APIs)"""
        # Try Hub API first (has better tag metadata)
        tags = self.get_tags_hub(namespace, repository)

        # If Hub API fails, try Registry API
        if not tags:
            self.log(f"  Trying Registry API as fallback...", "DEBUG")
            tags = self.get_tags_registry(namespace, repository)

        return tags

    @retry_with_backoff
    def delete_tag(self, namespace, repository, tag):
        """Delete a specific tag from a repository"""
        if self.dry_run:
            self.log(f"  🔍 [DRY RUN] Would delete: {namespace}/{repository}:{tag}")
            return True

        # Try Registry API delete first (more reliable)
        try:
            token = self.get_bearer_token(namespace, repository)

            # First, get the manifest digest
            manifest_url = f"{self.registry_url}/{namespace}/{repository}/manifests/{tag}"
            headers = {
                "Authorization": f"Bearer {token}",
                "Accept": "application/vnd.docker.distribution.manifest.v2+json"
            }

            response = requests.get(manifest_url, headers=headers, timeout=self.request_timeout)
            response.raise_for_status()

            # Get the digest from headers
            digest = response.headers.get('Docker-Content-Digest')
            if not digest:
                raise Exception("No digest found in manifest response")

            # Now delete by digest
            delete_url = f"{self.registry_url}/{namespace}/{repository}/manifests/{digest}"
            response = requests.delete(delete_url, headers=headers, timeout=self.request_timeout)
            response.raise_for_status()

            self.log(f"  ✅ Deleted: {namespace}/{repository}:{tag}")
            return True

        except Exception as e:
            self.log(f"  Registry delete failed, trying Hub API: {e}", "DEBUG")

            # Fall back to Hub API delete
            try:
                # URL encode the tag name to handle special characters
                encoded_tag = quote(tag, safe='')
                url = f"{self.hub_url}/repositories/{namespace}/{repository}/tags/{encoded_tag}"

                headers = {}
                if self.username and self.password:
                    headers["Authorization"] = self.get_basic_auth_header()

                response = requests.delete(url, headers=headers, timeout=self.request_timeout)
                response.raise_for_status()
                self.log(f"  ✅ Deleted via Hub API: {namespace}/{repository}:{tag}")
                return True

            except requests.exceptions.RequestException as e2:
                self.log(f"  ❌ Failed to delete {namespace}/{repository}:{tag}: {e2}", "ERROR")
                return False

    def test_authentication(self):
        """Test if authentication works"""
        self.log("🔐 Testing authentication...")

        # Test Hub API with basic auth to verify credentials
        try:
            url = f"{self.hub_url}/users/{self.username}"
            headers = {"Authorization": self.get_basic_auth_header()}
            response = requests.get(url, headers=headers, timeout=self.request_timeout)
            if response.status_code == 200:
                self.log("✅ Authentication successful")
                return True
        except Exception as e:
            self.log(f"⚠️  Authentication test failed: {e}", "DEBUG")

        self.log("❌ Authentication failed - please check your credentials", "ERROR")
        self.log("   Make sure DOCKERHUB_USERNAME and DOCKERHUB_PASSWORD are set correctly", "ERROR")
        self.log("   Password can be either your Docker Hub password or a Personal Access Token", "ERROR")
        return False

    def is_protected_tag(self, tag_name):
        """Check if a tag is protected from deletion"""
        # Built-in protected patterns
        protected_pattern = re.compile(r'^(latest|main|master|develop|\d+\.\d+\.\d+|v\d+\.\d+\.\d+|\d+\.\d+|\d+)$')

        # Check built-in patterns
        if protected_pattern.match(tag_name):
            return True

        # Check custom protected tags
        if tag_name in self.protected_tags:
            return True

        return False

    def should_delete_tag(self, tag_name, last_updated, pr_cutoff, sha_cutoff):
        """Determine if a tag should be deleted based on patterns and age"""
        # Patterns for different tag types
        pr_pattern = re.compile(r'^pr-\d+$')
        sha_pattern = re.compile(r'^(main|master|develop)-[a-f0-9]{7,}$')

        # Check custom patterns first
        for pattern_str, retention_days in self.custom_patterns.items():
            pattern = re.compile(pattern_str)
            if pattern.match(tag_name):
                cutoff = datetime.now(timezone.utc) - timedelta(days=retention_days)
                return last_updated < cutoff, "custom"

        # Check PR tags
        if pr_pattern.match(tag_name):
            return last_updated < pr_cutoff, "pr"

        # Check SHA tags
        if sha_pattern.match(tag_name):
            return last_updated < sha_cutoff, "sha"

        # Unknown format - don't delete
        return False, "unknown"

    def cleanup_repository(self, repo_spec, default_namespace=None, pr_retention_days=30, sha_retention_days=14):
        """
        Clean up old tags from a repository
        repo_spec can be either 'repository' or 'namespace/repository'
        """
        # Parse repository specification
        namespace, repository = self.parse_repository_spec(repo_spec)

        # Use default namespace if not specified in repo_spec
        if namespace is None:
            if default_namespace is None:
                self.log(f"❌ Error: No namespace specified for repository '{repository}'", "ERROR")
                self.log("   Use format 'namespace/repository' or set DOCKER_NAMESPACE", "ERROR")
                return None
            namespace = default_namespace

        self.log(f"\n📦 Processing repository: {namespace}/{repository}")

        tags = self.get_tags(namespace, repository)
        if not tags:
            self.log(f"  ℹ️  No tags found")
            repo_stats = {
                "repository": f"{namespace}/{repository}",
                "total_tags": 0,
                "protected": 0,
                "deleted": 0,
                "kept": 0,
                "failed": 0,
                "identified": 0
            }
            self.stats["repositories"].append(repo_stats)
            return repo_stats

        self.log(f"  📊 Found {len(tags)} total tags")

        # Calculate cutoff dates
        now = datetime.now(timezone.utc)
        pr_cutoff = now - timedelta(days=pr_retention_days)
        sha_cutoff = now - timedelta(days=sha_retention_days)

        deleted_count = 0
        protected_count = 0
        kept_count = 0
        failed_count = 0
        identified_count = 0

        for tag in tags:
            tag_name = tag.get("name")
            if not tag_name:
                continue

            # Parse last updated date
            last_updated_str = tag.get("last_updated", "")
            try:
                # Handle both ISO format and simple datetime
                if last_updated_str:
                    # Docker Hub dates end with 'Z' for UTC
                    last_updated = datetime.strptime(
                        last_updated_str[:19],
                        "%Y-%m-%dT%H:%M:%S"
                    ).replace(tzinfo=timezone.utc)
                else:
                    # If no date, assume it's old enough to consider
                    last_updated = datetime.now(timezone.utc) - timedelta(days=365)
            except (ValueError, TypeError):
                self.log(f"  ⚠️  Skipping {tag_name}: unable to parse date", "WARNING")
                kept_count += 1
                continue

            # Check if tag is protected
            if self.is_protected_tag(tag_name):
                self.log(f"  🛡️  Protected: {tag_name}")
                protected_count += 1
                self.stats["protected_count"] += 1
                continue

            # Check if tag should be deleted
            should_delete, tag_type = self.should_delete_tag(tag_name, last_updated, pr_cutoff, sha_cutoff)

            if should_delete:
                identified_count += 1
                self.stats["identified_count"] += 1

                if self.delete_tag(namespace, repository, tag_name):
                    deleted_count += 1
                    self.stats["deleted_count"] += 1
                else:
                    failed_count += 1
            else:
                if tag_type != "unknown":
                    self.log(f"  ⏳ Keeping {tag_type} tag (recent): {tag_name}")
                else:
                    self.log(f"  ❓ Keeping unknown format: {tag_name}")
                kept_count += 1

        # Summary
        self.log(f"\n  📈 Summary for {namespace}/{repository}:")
        self.log(f"     Protected: {protected_count}")
        self.log(f"     Identified: {identified_count}")
        self.log(f"     Deleted: {deleted_count}")
        self.log(f"     Kept: {kept_count}")
        if failed_count > 0:
            self.log(f"     Failed: {failed_count}", "WARNING")

        repo_stats = {
            "repository": f"{namespace}/{repository}",
            "total_tags": len(tags),
            "protected": protected_count,
            "deleted": deleted_count,
            "kept": kept_count,
            "failed": failed_count,
            "identified": identified_count
        }

        self.stats["repositories"].append(repo_stats)
        return repo_stats


def main():
    parser = argparse.ArgumentParser(description="Clean up old Docker Hub tags")
    parser.add_argument("--dry-run", action="store_true", help="Preview what would be deleted")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose logging")
    parser.add_argument("--pr-retention", type=int, default=30, help="Days to keep PR tags (default: 30)")
    parser.add_argument("--sha-retention", type=int, default=14, help="Days to keep SHA tags (default: 14)")
    parser.add_argument("--repositories", nargs="+", help="List of repositories to clean")
    parser.add_argument("--output-json", action="store_true", help="Output JSON summary to stdout")
    parser.add_argument("--protected-tags", nargs="*", help="Additional tags to protect from deletion")
    parser.add_argument("--custom-patterns", type=str, help="JSON string of custom patterns and retention days")

    args = parser.parse_args()

    # Get credentials from environment
    username = os.environ.get("DOCKERHUB_USERNAME")
    password = os.environ.get("DOCKERHUB_PASSWORD")
    namespace = os.environ.get("DOCKER_NAMESPACE", username)  # Default to username if not specified

    if not username or not password:
        print("❌ Error: DOCKERHUB_USERNAME and DOCKERHUB_PASSWORD environment variables are required", file=sys.stderr)
        print("   DOCKERHUB_PASSWORD can be either your Docker Hub password or a Personal Access Token", file=sys.stderr)
        sys.exit(2)  # Exit code 2 for configuration error

    if not args.repositories:
        print("❌ Error: No repositories specified. Use --repositories repo1 repo2 ...", file=sys.stderr)
        sys.exit(2)

    # Parse custom patterns if provided
    custom_patterns = {}
    if args.custom_patterns:
        try:
            custom_patterns = json.loads(args.custom_patterns)
        except json.JSONDecodeError as e:
            print(f"❌ Error: Invalid JSON for custom patterns: {e}", file=sys.stderr)
            sys.exit(2)

    # Get protected tags from environment if set
    protected_tags = args.protected_tags or []
    env_protected = os.environ.get("PROTECTED_TAGS", "")
    if env_protected:
        protected_tags.extend(env_protected.split(","))

    # Initialize cleaner
    cleaner = DockerHubCleaner(
        username,
        password,
        args.dry_run,
        args.verbose,
        protected_tags,
        custom_patterns
    )

    # Test authentication
    if not cleaner.test_authentication():
        sys.exit(2)  # Exit code 2 for authentication failure

    # Process each repository
    results = []
    failed_repos = []

    for repo_spec in args.repositories:
        try:
            result = cleaner.cleanup_repository(
                repo_spec,
                namespace,  # Use as default namespace for unqualified repos
                args.pr_retention,
                args.sha_retention
            )
            if result:
                results.append(result)

                # Track repositories with failures
                if result.get("failed", 0) > 0:
                    failed_repos.append(result["repository"])
            else:
                failed_repos.append(repo_spec)

        except Exception as e:
            cleaner.log(f"❌ Failed to process {repo_spec}: {e}", "ERROR")
            failed_repos.append(repo_spec)
            # Add a failure result for this repository
            results.append({
                "repository": repo_spec,
                "total_tags": 0,
                "protected": 0,
                "deleted": 0,
                "kept": 0,
                "failed": -1,  # -1 indicates complete failure
                "identified": 0
            })

    # Output JSON if requested
    if args.output_json:
        # Output clean JSON to stdout for the action to parse
        json.dump(cleaner.stats, sys.stdout, indent=2)
    else:
        # Print human-readable summary to stderr
        print("\n" + "="*60, file=sys.stderr)
        print("🎯 CLEANUP COMPLETE", file=sys.stderr)
        print("="*60, file=sys.stderr)

        if args.dry_run:
            print("ℹ️  This was a DRY RUN - no tags were actually deleted", file=sys.stderr)

        # Calculate totals
        total_deleted = sum(r.get("deleted", 0) for r in results)
        total_kept = sum(r.get("kept", 0) for r in results)
        total_protected = sum(r.get("protected", 0) for r in results)
        total_failed = sum(r.get("failed", 0) for r in results if r.get("failed", 0) > 0)
        total_identified = sum(r.get("identified", 0) for r in results)

        print(f"\n📊 Overall Statistics:", file=sys.stderr)
        print(f"   Repositories processed: {len(results)}", file=sys.stderr)
        print(f"   Tags identified: {total_identified}", file=sys.stderr)
        print(f"   Tags deleted: {total_deleted}", file=sys.stderr)
        print(f"   Tags kept: {total_kept}", file=sys.stderr)
        print(f"   Tags protected: {total_protected}", file=sys.stderr)

        if total_failed > 0:
            print(f"   ⚠️  Tags failed to delete: {total_failed}", file=sys.stderr)

        if failed_repos:
            print(f"\n❌ Failed repositories:", file=sys.stderr)
            for repo in failed_repos:
                print(f"   - {repo}", file=sys.stderr)

        if args.dry_run and total_identified > 0:
            print(f"\n💡 To actually delete these {total_identified} tags, run without --dry-run", file=sys.stderr)

    # Exit with appropriate code
    if failed_repos:
        sys.exit(1)  # Exit code 1 for partial failure
    else:
        sys.exit(0)  # Exit code 0 for success


if __name__ == "__main__":
    main()
