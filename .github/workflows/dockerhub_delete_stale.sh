#!/bin/bash
set -e
source env.sh  #@@@

# Docker Hub Untagged Image Cleanup Script.
# Finds and deletes untagged Docker images from Docker Hub repositories.
# Based on gist.github.com/LuisAlejandro/e73e7b6435e6c346104950f5587b8d3a

# Secrets from GitHub secrets are mapped as:
# USERNAME: '${{ secrets.DOCKERHUB_USERNAME }}'
# PASSWORD: '${{ secrets.DOCKERHUB_PASSWORD }}'
# DOCKER_HUB_COOKIE: '${{ secrets.DOCKERHUB_COOKIE }}'

# Configuration
REPO='tripalproject/tripaldocker-drupal tripalproject/tripaldocker'
#REPO='tripalproject/tripaldocker'
MAX_UNTAGGED_LIMIT=250
MAX_PAGINATION_REQUESTS=30

# Functions to print output
print_status() {
    echo "[INFO] $1" >&2
}

print_warning() {
    echo "[WARNING] $1" >&2
}

print_error() {
    echo "[ERROR] $1" >&2
}

# Function to get bearer token for Registry API
get_token() {
    local repo=$1
    local token=$(curl -s -u "$USERNAME:$PASSWORD" \
        "https://auth.docker.io/token?service=registry.docker.io&scope=repository:$repo:pull,push,delete" |
        jq -r .token)

    if [ "$token" = "null" ] || [ -z "$token" ]; then
        print_error "Failed to get authentication token for $repo. Check your credentials."
        exit 1
    fi

    echo "$token"
}

# Function to list all tags for the repository
list_tags() {
    local token=$1
    local repo=$2
    local tags=$(curl -s -H "Authorization: Bearer $token" \
        "https://registry-1.docker.io/v2/$repo/tags/list" | jq -r '.tags[]?' 2>/dev/null)

    echo "$tags"
}

# Function to get digest for a specific tag
get_digest_for_tag() {
    local token=$1
    local repo=$2
    local tag=$3

    local digest=$(curl -sI -H "Authorization: Bearer $token" \
        -H "Accept: application/vnd.docker.distribution.manifest.v2+json,application/vnd.oci.image.manifest.v1+json,application/vnd.oci.image.index.v1+json" \
        "https://registry-1.docker.io/v2/$repo/manifests/$tag" |
        grep -i docker-content-digest | tr -d '\r' | awk '{print $2}')

    echo "$digest"
}

# Function to check rate limits and wait if necessary
check_rate_limit() {
    local headers_file=$1
    local request_num=$2

    # Extract rate limit headers
    local limit=$(grep -i "x-ratelimit-limit" "$headers_file" | awk '{print $2}' | tr -d '\r')
    local remaining=$(grep -i "x-ratelimit-remaining" "$headers_file" | awk '{print $2}' | tr -d '\r')
    local reset=$(grep -i "x-ratelimit-reset" "$headers_file" | awk '{print $2}' | tr -d '\r')

    if [ -n "$limit" ] && [ -n "$remaining" ] && [ -n "$reset" ]; then
        echo "Request $request_num: Rate limit status - $remaining/$limit remaining" >&2

        # Check if we're approaching the rate limit (less than 5 requests remaining)
        if [ "$remaining" -le 5 ]; then
            local current_time=$(date +%s)
            local wait_time=$((reset - current_time))

            if [ $wait_time -gt 0 ]; then
                print_warning "Rate limit nearly exhausted ($remaining/$limit remaining)"
                print_warning "Waiting for rate limit reset in $wait_time seconds..."

                # Show countdown
                while [ $wait_time -gt 0 ]; do
                    local minutes=$((wait_time / 60))
                    local seconds=$((wait_time % 60))
                    printf "\rTime remaining: %02d:%02d" $minutes $seconds
                    sleep 1
                    wait_time=$((wait_time - 1))
                done

                echo ""
                print_status "Rate limit reset. Continuing..."
            else
                print_status "Rate limit should have already reset. Continuing..."
            fi
        fi
    else
        echo "Request $request_num: No rate limit headers found" >&2
    fi
}

# Function to get all manifests using Docker Hub's internal API
list_all_manifests() {
    local repo=$1
    local request_num=1
    local all_manifests=""
    local last_key=""

    while true; do
        local response
        local headers_file="/tmp/dockerhub_headers_$$"

        if [ $request_num -eq 1 ]; then
            # First request: GET without pagination
            echo "Request $request_num: Initial GET request" >&2
            response=$(curl -s -D "$headers_file" -H "Cookie: $DOCKER_HUB_COOKIE" \
                -H "X-Requested-With: XMLHttpRequest" \
                -H "Accept: application/json" \
                "https://hub.docker.com/repository/docker/$repo/image-management.data?sortField=last_pushed&sortOrder=asc")
        else
            # Subsequent requests: POST with lastEvaluatedKey
            echo "Request $request_num: POST request with pagination" >&2
            response=$(curl -s -D "$headers_file" -X POST \
                -H "Cookie: $DOCKER_HUB_COOKIE" \
                -H "X-Requested-With: XMLHttpRequest" \
                -H "Accept: application/json" \
                -H "Content-Type: application/x-www-form-urlencoded" \
                --data-raw "intent=paginate&lastEvaluatedKey=$last_key" \
                "https://hub.docker.com/repository/docker/$repo/image-management.data?sortField=last_pushed&sortOrder=asc")
        fi

        # Check rate limits and wait if necessary
        check_rate_limit "$headers_file" "$request_num"

        # Clean up headers file
        rm -f "$headers_file"

        # Extract manifest digests from the flat array response
        local manifests=$(echo "$response" | jq -r '.[] | select(type == "string" and (startswith("sha256:") or startswith("sha1:") or startswith("md5:"))) // empty' 2>/dev/null)

        # Debug: show what we found on this request
        if [ -n "$manifests" ]; then
            echo "Request $request_num: Found $(echo "$manifests" | wc -w) digest(s)" >&2
            all_manifests="$all_manifests $manifests"
        else
            echo "Request $request_num: No digests found" >&2
        fi

        # Check if there's a lastEvaluatedKey for pagination
        local last_key_index=$(echo "$response" | jq -r 'to_entries | map(select(.value == "lastEvaluatedKey")) | .[0].key // empty' 2>/dev/null)
        last_key=""
        if [ -n "$last_key_index" ] && [ "$last_key_index" != "null" ]; then
            # Get the next element after "lastEvaluatedKey"
            last_key=$(echo "$response" | jq -r ".[$((last_key_index + 1))] // empty" 2>/dev/null)
        fi

        if [ -z "$last_key" ] || [ "$last_key" = "null" ]; then
            echo "Request $request_num: No more pages (no lastEvaluatedKey found)" >&2
            break
        else
            echo "Request $request_num: Found lastEvaluatedKey, continuing to next request..." >&2
        fi

        request_num=$((request_num + 1))

        # Safety break to prevent infinite loops
        if [ $request_num -gt $MAX_PAGINATION_REQUESTS ]; then
            echo "Reached maximum requests ($MAX_PAGINATION_REQUESTS). Stopping." >&2
            break
        fi
    done

    echo "$all_manifests" | tr ' ' '\n' | sort | uniq | grep -v '^$'
}

# Function to delete manifest by digest
delete_manifest() {
    local token=$1
    local repo=$2
    local digest=$3

    local response=$(curl -s -w "%{http_code}" -X DELETE \
        -H "Authorization: Bearer $token" \
        "https://registry-1.docker.io/v2/$repo/manifests/$digest")

    local http_code="${response: -3}"

    if [ "$http_code" = "202" ] || [ "$http_code" = "204" ]; then
        print_status "Successfully deleted manifest: $digest"
        return 0
    else
        print_error "Failed to delete manifest: $digest (HTTP: $http_code)"
        return 1
    fi
}

# Function to find untagged manifests
find_untagged_manifests() {
    local token=$1
    local repo=$2
    local limit=${3:-0}

    print_status "Getting tagged manifests..."
    local tags=$(list_tags "$token" "$repo")
    local tagged_digests=""

    if [ -n "$tags" ]; then
        while IFS= read -r tag; do
            if [ -n "$tag" ]; then
                local digest=$(get_digest_for_tag "$token" "$repo" "$tag")
                if [ -n "$digest" ]; then
                    tagged_digests="$tagged_digests $digest"
                fi
            fi
        done <<<"$tags"
    fi

    print_status "Getting all manifests (including untagged)..."
    local all_manifests=$(list_all_manifests "$repo")

    print_status "Finding untagged manifests..."
    local untagged_manifests=""
    local count=0

    while IFS= read -r manifest; do
        if [ -n "$manifest" ]; then
            if ! echo "$tagged_digests" | grep -q "$manifest"; then
                untagged_manifests="$untagged_manifests $manifest"
                count=$((count + 1))

                # Stop if we've reached the limit
                if [ "$limit" -gt 0 ] && [ "$count" -ge "$limit" ]; then
                    print_warning "Reached limit of $limit untagged manifests. There may be more."
                    break
                fi
            fi
        fi
    done <<<"$all_manifests"

    echo "$untagged_manifests" | tr ' ' '\n' | grep -v '^$'
}

# Function to show usage
show_usage() {
    cat <<EOF
Docker Hub Untagged Image Cleanup Script

Usage: $0 COMMAND

Commands:
    list-untagged       List untagged manifests (digests)
    delete-untagged     Delete all untagged manifests
    -h, --help          Show this help message

Environment Variables:
    USERNAME             Docker Hub username (required)
    PASSWORD             Docker Hub password or personal access token (required)
    DOCKER_HUB_COOKIE    Session cookie from logged-in browser (required)
    REPO                 Space-separated list of repositories (required)
    MAX_UNTAGGED_LIMIT   Maximum number of untagged manifests to list (default: 100)
    MAX_PAGINATION_REQUESTS Maximum number of pagination requests to make (default: 10)

Examples:
    REPO="dockershelf/python" $0 list-untagged
    REPO="dockershelf/python dockershelf/node" $0 delete-untagged

    # With custom repositories:
    REPO="dockershelf/python dockershelf/node dockershelf/go" $0 list-untagged

Getting the session cookie:
    1. Open Docker Hub in your browser and log in
    2. Open Developer Tools (F12)
    3. Go to Network tab and refresh the page
    4. Click on any request to hub.docker.com
    5. In Request Headers, copy the entire 'Cookie:' value
    6. Add it to your .env file as DOCKER_HUB_COOKIE="your-cookie-here"

EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
    -h | --help)
        show_usage
        exit 0
        ;;
    list-untagged | delete-untagged)
        COMMAND="$1"
        shift
        ;;
    *)
        print_error "Unknown option: $1"
        show_usage
        exit 1
        ;;
    esac
done

# Validate required parameters
if [ -z "$USERNAME" ] || [ -z "$PASSWORD" ]; then
    print_error "Missing required credentials. Please set USERNAME and PASSWORD environment variables."
    exit 1
fi

if [ -z "$REPO" ]; then
    print_error "Missing required REPO variable. Please set REPO environment variable."
    print_error "Example: REPO=\"dockershelf/python dockershelf/node dockershelf/go\""
    exit 1
fi

if [ -z "$COMMAND" ]; then
    print_error "No command specified."
    show_usage
    exit 1
fi

if [ -z "$DOCKER_HUB_COOKIE" ]; then
    print_error "Docker Hub session cookie is required."
    print_error "Please provide it using -c/--cookie or DOCKER_HUB_COOKIE env var."
    print_error ""
    print_error "To get the cookie:"
    print_error "1. Open Docker Hub in your browser and log in"
    print_error "2. Open Developer Tools (F12)"
    print_error "3. Go to Network tab and refresh the page"
    print_error "4. Click on any request to hub.docker.com"
    print_error "5. In Request Headers, copy the entire 'Cookie:' value"
    exit 1
fi

# Validate limit parameter
if ! [[ "$MAX_UNTAGGED_LIMIT" =~ ^[0-9]+$ ]] || [ "$MAX_UNTAGGED_LIMIT" -lt 1 ]; then
    print_error "Invalid limit value: $MAX_UNTAGGED_LIMIT. Must be a positive integer."
    exit 1
fi

# Check if jq is installed
if ! command -v jq &>/dev/null; then
    print_error "jq is required but not installed. Please install jq first."
    exit 1
fi

# Execute the requested command for each repository
for current_repo in $REPO; do
    print_status "Working with repository: $current_repo"

    # Get authentication token for this repository
    TOKEN=$(get_token "$current_repo")

    case $COMMAND in
    list-untagged)
        print_status "Listing untagged manifests (limit: $MAX_UNTAGGED_LIMIT)..."
        untagged=$(find_untagged_manifests "$TOKEN" "$current_repo" "$MAX_UNTAGGED_LIMIT")
        if [ -n "$untagged" ]; then
            echo "=== $current_repo ==="
            echo "$untagged"
            echo ""
            print_status "Found $(echo "$untagged" | wc -w) untagged manifest(s) in $current_repo"
        else
            print_warning "No untagged manifests found in $current_repo."
        fi
        echo ""
        ;;

    delete-untagged)
        # For deletion, get ALL untagged manifests (no limit)
        print_status "Getting complete list of untagged manifests for deletion..."
        untagged=$(find_untagged_manifests "$TOKEN" "$current_repo" 0)

        if [ -n "$untagged" ]; then
            deleted_count=0
            failed_count=0
            total_count=$(echo "$untagged" | wc -w)

            print_status "Deleting $total_count untagged manifest(s) from $current_repo..."

            while IFS= read -r digest; do
                if [ -n "$digest" ]; then
                    if delete_manifest "$TOKEN" "$current_repo" "$digest"; then
                        deleted_count=$((deleted_count + 1))
                    else
                        failed_count=$((failed_count + 1))
                    fi
                fi
            done <<<"$untagged"

            print_status "Deletion complete for $current_repo. Deleted: $deleted_count, Failed: $failed_count"
        else
            print_warning "No untagged manifests found to delete in $current_repo."
        fi
        echo ""
        ;;
    esac
done

print_status "Operation completed."
