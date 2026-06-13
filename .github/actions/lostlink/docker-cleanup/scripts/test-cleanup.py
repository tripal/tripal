#!/usr/bin/env python3
"""
Test script for Docker Hub Cleanup
Tests the logic without making actual API calls
"""

import re
from datetime import datetime, timedelta, timezone


def test_patterns():
    """Test regex patterns for tag matching"""
    
    # Patterns from the cleanup script
    pr_pattern = re.compile(r'^pr-\d+$')
    sha_pattern = re.compile(r'^(main|master|develop)-[a-f0-9]{7,}$')
    protected_pattern = re.compile(r'^(latest|main|master|develop|\d+\.\d+\.\d+|v\d+\.\d+\.\d+|\d+\.\d+|\d+)$')
    
    # Test cases
    test_cases = [
        # Tag name, Expected result (pr/sha/protected/none)
        ("pr-123", "pr"),
        ("pr-456789", "pr"),
        ("main-abc123f", "sha"),
        ("master-1234567", "sha"),
        ("develop-abcdef0", "sha"),
        ("latest", "protected"),
        ("main", "protected"),
        ("master", "protected"),
        ("develop", "protected"),
        ("1.0.0", "protected"),
        ("v1.0.0", "protected"),
        ("1.0", "protected"),
        ("1", "protected"),
        ("2", "protected"),
        ("feature-branch", "none"),
        ("pr-abc", "none"),  # Invalid PR format
        ("main-xyz", "none"),  # Invalid SHA (not hex)
        ("main-12345", "none"),  # Invalid SHA (not hex)
        ("v1", "none"),  # v1 without dots is not protected
        ("1.0.0.0", "none"),  # Too many version parts
        ("latest-l11", "none"),  # Not a protected pattern
        ("latest-v2", "none"),  # Similar variant, not protected
    ]
    
    print("Testing tag patterns...")
    print("-" * 60)
    
    passed = 0
    failed = 0
    
    for tag_name, expected in test_cases:
        if pr_pattern.match(tag_name):
            result = "pr"
        elif sha_pattern.match(tag_name):
            result = "sha"
        elif protected_pattern.match(tag_name):
            result = "protected"
        else:
            result = "none"
        
        status = "✅" if result == expected else "❌"
        if result == expected:
            passed += 1
        else:
            failed += 1
            
        print(f"{status} Tag: {tag_name:20} Expected: {expected:10} Got: {result:10}")
    
    print("-" * 60)
    print(f"Results: {passed} passed, {failed} failed")
    return failed == 0


def test_date_parsing():
    """Test date parsing logic"""
    
    print("\nTesting date parsing...")
    print("-" * 60)
    
    test_dates = [
        ("2024-01-15T10:30:45.123456Z", True),
        ("2024-01-15T10:30:45", True),
        ("2024-01-15T10:30:45Z", True),
        ("invalid-date", False),
        ("", False),
        (None, False),
    ]
    
    passed = 0
    failed = 0
    
    for date_str, should_parse in test_dates:
        try:
            if date_str:
                parsed = datetime.strptime(date_str[:19], "%Y-%m-%dT%H:%M:%S")
                result = True
            else:
                result = False
        except (ValueError, TypeError):
            result = False
        
        status = "✅" if result == should_parse else "❌"
        if result == should_parse:
            passed += 1
        else:
            failed += 1
            
        print(f"{status} Date: {str(date_str):30} Should parse: {should_parse:5} Result: {result}")
    
    print("-" * 60)
    print(f"Results: {passed} passed, {failed} failed")
    return failed == 0


def test_retention_logic():
    """Test retention date logic"""
    
    print("\nTesting retention logic...")
    print("-" * 60)
    
    now = datetime.now(timezone.utc)
    pr_retention_days = 30
    sha_retention_days = 14
    
    pr_cutoff = now - timedelta(days=pr_retention_days)
    sha_cutoff = now - timedelta(days=sha_retention_days)
    
    test_cases = [
        # (tag_type, days_old, should_delete)
        ("pr", 31, True),   # Old PR tag
        ("pr", 29, False),  # Recent PR tag
        ("pr", 30, False),  # Exactly at cutoff (kept)
        ("sha", 15, True),  # Old SHA tag
        ("sha", 13, False), # Recent SHA tag
        ("sha", 14, False), # Exactly at cutoff (kept)
    ]
    
    passed = 0
    failed = 0
    
    for tag_type, days_old, should_delete in test_cases:
        tag_date = now - timedelta(days=days_old)
        
        if tag_type == "pr":
            would_delete = tag_date < pr_cutoff
        else:  # sha
            would_delete = tag_date < sha_cutoff
        
        status = "✅" if would_delete == should_delete else "❌"
        if would_delete == should_delete:
            passed += 1
        else:
            failed += 1
            
        print(f"{status} Type: {tag_type:3} Days old: {days_old:3} Should delete: {should_delete:5} Result: {would_delete}")
    
    print("-" * 60)
    print(f"Results: {passed} passed, {failed} failed")
    return failed == 0


def test_url_encoding():
    """Test URL encoding for special characters"""
    from urllib.parse import quote
    
    print("\nTesting URL encoding...")
    print("-" * 60)
    
    test_cases = [
        ("normal-tag", "normal-tag"),
        ("tag/with/slash", "tag%2Fwith%2Fslash"),
        ("tag+plus", "tag%2Bplus"),
        ("tag with space", "tag%20with%20space"),
        ("tag@special", "tag%40special"),
        ("tag#hash", "tag%23hash"),
    ]
    
    passed = 0
    failed = 0
    
    for tag_name, expected_encoded in test_cases:
        encoded = quote(tag_name, safe='')
        status = "✅" if encoded == expected_encoded else "❌"
        
        if encoded == expected_encoded:
            passed += 1
        else:
            failed += 1
            
        print(f"{status} Tag: {tag_name:20} Expected: {expected_encoded:30} Got: {encoded}")
    
    print("-" * 60)
    print(f"Results: {passed} passed, {failed} failed")
    return failed == 0


def test_repository_parsing():
    """Test repository specification parsing (namespace/repository)"""
    
    print("\nTesting repository specification parsing...")
    print("-" * 60)
    
    def parse_repository_spec(repo_spec):
        """Parse repository specification to extract namespace and repository name."""
        parts = repo_spec.split('/')
        if len(parts) == 2:
            # Format: namespace/repository
            return parts[0], parts[1]
        elif len(parts) == 1:
            # Format: repository (namespace must be provided separately)
            return None, parts[0]
        else:
            return None, None  # Invalid format
    
    test_cases = [
        # (input, expected_namespace, expected_repo)
        ("myrepo", None, "myrepo"),
        ("namespace/myrepo", "namespace", "myrepo"),
        ("org/team/repo", None, None),  # Invalid - too many parts
        ("", None, ""),  # Edge case - empty string
        ("namespace/", "namespace", ""),  # Edge case - trailing slash
        ("/repo", "", "repo"),  # Edge case - leading slash
        ("docker.io/namespace/repo", None, None),  # Invalid - registry included
        ("myorg/webapp", "myorg", "webapp"),  # Example org/repo format
        ("company/service", "company", "service"),  # Example company/service format
    ]
    
    passed = 0
    failed = 0
    
    for repo_spec, expected_ns, expected_repo in test_cases:
        namespace, repository = parse_repository_spec(repo_spec)
        
        if namespace == expected_ns and repository == expected_repo:
            status = "✅"
            passed += 1
        else:
            status = "❌"
            failed += 1
        
        # Format None values as "None" for display
        exp_ns = str(expected_ns) if expected_ns is not None else "None"
        exp_repo = str(expected_repo) if expected_repo is not None else "None"
        got_ns = str(namespace) if namespace is not None else "None"
        got_repo = str(repository) if repository is not None else "None"
        
        print(f"{status} Input: {repo_spec:25} Expected: {exp_ns:10}/{exp_repo:15} Got: {got_ns:10}/{got_repo}")
    
    print("-" * 60)
    print(f"Results: {passed} passed, {failed} failed")
    return failed == 0


def main():
    """Run all tests"""
    print("=" * 60)
    print("Docker Hub Cleanup Logic Tests")
    print("=" * 60)
    
    all_passed = True
    
    # Run tests
    all_passed = test_patterns() and all_passed
    all_passed = test_date_parsing() and all_passed
    all_passed = test_retention_logic() and all_passed
    all_passed = test_url_encoding() and all_passed
    all_passed = test_repository_parsing() and all_passed
    
    # Summary
    print("\n" + "=" * 60)
    if all_passed:
        print("✅ All tests passed!")
    else:
        print("❌ Some tests failed. Please review the results above.")
    print("=" * 60)
    
    return 0 if all_passed else 1


if __name__ == "__main__":
    exit(main())