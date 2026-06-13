# Docker Hub Cleanup Action

[![GitHub Action](https://img.shields.io/badge/GitHub-Action-blue?logo=github)](https://github.com/marketplace/actions/docker-hub-cleanup)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A GitHub Action to automatically clean up old Docker tags from Docker Hub repositories, helping you manage storage costs and keep your repositories organized.

## 🎯 Features

- **🗑️ Automated Cleanup**: Remove old PR tags, branch SHA tags, and custom patterns
- **🛡️ Protected Tags**: Never delete important tags (latest, main, semantic versions)
- **🔐 Secure Authentication**: Works with both Docker Hub passwords and Personal Access Tokens
- **📊 Detailed Reporting**: Get summaries and statistics of cleanup operations
- **🎮 Flexible Control**: Dry-run mode, custom retention periods, and pattern matching
- **🔄 Multiple Repositories**: Clean multiple Docker repositories in a single run
- **📈 Output Metrics**: Access cleanup statistics as action outputs for further processing

## 🚀 Quick Start

### Basic Usage

```yaml
name: Docker Cleanup
on:
  schedule:
    - cron: '0 2 * * 0'  # Weekly on Sunday at 2 AM UTC
  workflow_dispatch:  # Allow manual trigger

jobs:
  cleanup:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Docker Hub Cleanup
        uses: lostlink/docker-cleanup@v1
        with:
          username: ${{ secrets.DOCKERHUB_USERNAME }}
          password: ${{ secrets.DOCKERHUB_PASSWORD }}
          repositories: 'image1,image2,image3'
          dry-run: false  # Set to true for testing
```

### Advanced Usage

```yaml
- name: Docker Hub Cleanup
  uses: lostlink/docker-cleanup@v1
  with:
    username: ${{ secrets.DOCKERHUB_USERNAME }}
    password: ${{ secrets.DOCKERHUB_PASSWORD }}
    organization: 'my-org'  # Optional: defaults to username
    repositories: 'app-frontend,app-backend,app-worker'
    pr-retention-days: 30  # Keep PR tags for 30 days
    sha-retention-days: 14  # Keep SHA tags for 14 days
    dry-run: false
    verbose: true  # Enable detailed logging
    protected-tags: 'staging,production'  # Additional protected tags
    custom-patterns: '{"^feature-.*": 7, "^hotfix-.*": 3}'  # Custom patterns with retention days
```

### Multiple Organizations/Namespaces

You can clean repositories from different organizations by specifying the full namespace:

```yaml
- name: Docker Hub Cleanup
  uses: lostlink/docker-cleanup@v1
  with:
    username: ${{ secrets.DOCKERHUB_USERNAME }}
    password: ${{ secrets.DOCKERHUB_PASSWORD }}
    # Mix repositories from different namespaces
    repositories: 'myorg/frontend,mycompany/backend,personal-repo'
    dry-run: false
```

When specifying repositories:
- Use `namespace/repository` format to access repositories in specific organizations
- Use just `repository` to use the default namespace (organization input or username)
- You can mix both formats in the same repositories list

## 📋 Prerequisites

### GitHub Secrets

Add these secrets to your repository:

1. Go to **Settings** → **Secrets and variables** → **Actions**
2. Add the following secrets:
   - `DOCKERHUB_USERNAME`: Your Docker Hub username
   - `DOCKERHUB_PASSWORD`: Your Docker Hub password or [Personal Access Token](#personal-access-token)

### Personal Access Token

For enhanced security, use a Docker Hub Personal Access Token instead of your password:

1. Log in to [Docker Hub](https://hub.docker.com)
2. Go to **Account Settings** → **Security** → **Personal Access Tokens**
3. Click **Generate New Token**
4. Give it a descriptive name (e.g., "GitHub Actions Cleanup")
5. Select appropriate permissions:
   - For public repositories: `Public Repo Write`
   - For private repositories: `Private Repo Write`
6. Copy the token and save it as `DOCKERHUB_PASSWORD` in GitHub Secrets

## ⚙️ Configuration

### Inputs

| Input | Description | Required | Default |
|-------|-------------|----------|---------|
| `username` | Docker Hub username | ✅ | - |
| `password` | Docker Hub password or Personal Access Token | ✅ | - |
| `organization` | Default Docker Hub organization/namespace | ❌ | `username` |
| `repositories` | Comma-separated list of repository names (can include namespace) | ✅ | - |
| `pr-retention-days` | Days to retain PR tags (pr-*) | ❌ | `30` |
| `sha-retention-days` | Days to retain branch SHA tags | ❌ | `14` |
| `dry-run` | Preview deletions without executing | ❌ | `true` |
| `verbose` | Enable verbose logging | ❌ | `false` |
| `protected-tags` | Additional tags to protect | ❌ | `''` |
| `custom-patterns` | JSON string of custom patterns | ❌ | `'{}'` |

### Outputs

| Output | Description | Example |
|--------|-------------|---------|
| `deleted-count` | Total number of tags deleted | `42` |
| `identified-count` | Tags identified for deletion | `50` |
| `protected-count` | Tags protected from deletion | `10` |
| `summary` | JSON summary of the operation | `{...}` |

## 🛡️ Protected Tags

The following tags are **always** protected from deletion:

- `latest`
- `main`, `master`, `develop`
- Semantic version tags: `v1.0.0`, `1.0.0`, `1.0`, `1`
- Any tags specified in `protected-tags` input

## 📝 Cleanup Patterns

### Default Patterns

| Pattern | Example | Default Retention |
|---------|---------|-------------------|
| PR tags | `pr-123`, `pr-456` | 30 days |
| Branch SHA tags | `main-abc123f`, `develop-xyz789a` | 14 days |

### Custom Patterns

Define custom patterns with specific retention periods:

```yaml
custom-patterns: |
  {
    "^feature-.*": 7,
    "^hotfix-.*": 3,
    "^release-.*": 60,
    "^test-.*": 1
  }
```

## 🔄 Common Use Cases

### Weekly Automated Cleanup

```yaml
name: Weekly Docker Cleanup
on:
  schedule:
    - cron: '0 2 * * 0'  # Every Sunday at 2 AM UTC

jobs:
  cleanup:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Clean Docker Hub
        uses: lostlink/docker-cleanup@v1
        with:
          username: ${{ secrets.DOCKERHUB_USERNAME }}
          password: ${{ secrets.DOCKERHUB_PASSWORD }}
          repositories: 'my-app'
          pr-retention-days: 30
          sha-retention-days: 7
          dry-run: false
```

### Manual Cleanup with Dry Run

```yaml
name: Manual Docker Cleanup
on:
  workflow_dispatch:
    inputs:
      dry_run:
        description: 'Dry run mode'
        required: true
        default: 'true'
        type: choice
        options:
          - 'true'
          - 'false'

jobs:
  cleanup:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Clean Docker Hub
        uses: lostlink/docker-cleanup@v1
        with:
          username: ${{ secrets.DOCKERHUB_USERNAME }}
          password: ${{ secrets.DOCKERHUB_PASSWORD }}
          repositories: 'app1,app2,app3'
          dry-run: ${{ github.event.inputs.dry_run }}
          verbose: true
```

### Multi-Repository Matrix Cleanup

```yaml
name: Matrix Docker Cleanup
on:
  schedule:
    - cron: '0 2 * * 0'

jobs:
  cleanup:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        repository:
          - name: frontend
            pr-retention: 30
            sha-retention: 14
          - name: backend
            pr-retention: 60
            sha-retention: 30
          - name: worker
            pr-retention: 14
            sha-retention: 7
    
    steps:
      - uses: actions/checkout@v4
      
      - name: Clean ${{ matrix.repository.name }}
        uses: lostlink/docker-cleanup@v1
        with:
          username: ${{ secrets.DOCKERHUB_USERNAME }}
          password: ${{ secrets.DOCKERHUB_PASSWORD }}
          repositories: ${{ matrix.repository.name }}
          pr-retention-days: ${{ matrix.repository.pr-retention }}
          sha-retention-days: ${{ matrix.repository.sha-retention }}
          dry-run: false
```

### Using Action Outputs

```yaml
- name: Clean Docker Hub
  id: cleanup
  uses: lostlink/docker-cleanup@v1
  with:
    username: ${{ secrets.DOCKERHUB_USERNAME }}
    password: ${{ secrets.DOCKERHUB_PASSWORD }}
    repositories: 'my-app'
    dry-run: false

- name: Process Results
  run: |
    echo "Deleted ${{ steps.cleanup.outputs.deleted-count }} tags"
    echo "Protected ${{ steps.cleanup.outputs.protected-count }} tags"
    
    # Parse JSON summary
    echo '${{ steps.cleanup.outputs.summary }}' | jq '.'
    
- name: Send Notification
  if: steps.cleanup.outputs.deleted-count > 50
  run: |
    echo "Large cleanup performed: ${{ steps.cleanup.outputs.deleted-count }} tags deleted"
```

## 🧪 Testing

### Test with Dry Run

Always test with dry-run mode first:

```yaml
- uses: lostlink/docker-cleanup@v1
  with:
    username: ${{ secrets.DOCKERHUB_USERNAME }}
    password: ${{ secrets.DOCKERHUB_PASSWORD }}
    repositories: 'test-repo'
    dry-run: true  # Preview only
    verbose: true  # Detailed output
```

### Local Testing

Test the cleanup script locally:

```bash
# Clone the action repository
git clone https://github.com/lostlink/docker-cleanup.git
cd docker-cleanup

# Set environment variables
export DOCKERHUB_USERNAME=your-username
export DOCKERHUB_PASSWORD=your-password-or-token
export DOCKER_NAMESPACE=your-org  # Optional: default namespace for unqualified repos

# Run with dry-run
python scripts/dockerhub-cleanup.py \
  --repositories repo1 repo2 \
  --pr-retention 30 \
  --sha-retention 14 \
  --dry-run \
  --verbose

# Or specify repositories with their namespace
python scripts/dockerhub-cleanup.py \
  --repositories myorg/repo1 anotherorg/repo2 personal-repo \
  --dry-run \
  --verbose
```

## 🚨 Troubleshooting

### Common Issues

#### "Unauthorized" or "403 Forbidden"
- Verify your Docker Hub credentials are correct
- Ensure the Personal Access Token has write permissions
- Check that the username matches your Docker Hub account exactly

#### "Repository not found"
- Verify the repository names are correct
- Check the organization/namespace setting
- For repositories in other organizations, use the full `namespace/repository` format
- Ensure repositories exist and are accessible with your credentials

#### No tags deleted
- Verify tags meet deletion criteria (age and pattern)
- Check if running in dry-run mode
- Review retention period settings
- Check verbose logs for details

#### Rate limiting
- The action implements exponential backoff for rate limits
- Consider running cleanup during off-peak hours
- Reduce frequency if hitting limits regularly

### Debug Mode

Enable verbose logging for detailed information:

```yaml
- uses: lostlink/docker-cleanup@v1
  with:
    username: ${{ secrets.DOCKERHUB_USERNAME }}
    password: ${{ secrets.DOCKERHUB_PASSWORD }}
    repositories: 'my-app'
    verbose: true  # Enable debug output
    dry-run: true  # Safe testing
```

## 📊 Performance

- **Execution Time**: ~2-5 minutes per repository (depending on tag count)
- **API Calls**: ~10-50 per repository
- **Rate Limits**: Handles Docker Hub rate limits with automatic retry
- **Storage Savings**: Typically 50-80% reduction in stored tags

## 🔒 Security

- Credentials are never logged or exposed
- Supports Docker Hub Personal Access Tokens
- Uses GitHub Secrets for secure storage
- Implements secure authentication with Docker Registry v2
- All operations use HTTPS

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 🐛 Issues

If you encounter any problems, please [file an issue](https://github.com/lostlink/docker-cleanup/issues) along with a detailed description.

## 🙏 Acknowledgments

- Docker Hub API documentation
- GitHub Actions community
- Contributors and users of this action

## 📈 Roadmap

- [ ] Support for additional container registries (GitHub Container Registry, AWS ECR)
- [ ] Webhook notifications for cleanup summaries
- [ ] Advanced filtering options
- [ ] Cleanup based on image size
- [ ] Support for keeping N most recent tags

---

Made with ❤️ for the Docker community