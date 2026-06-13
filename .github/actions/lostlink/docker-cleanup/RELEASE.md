# v1.0.1: Multi-Namespace Support & Enhanced Repository Access

## 🎯 Overview

This release introduces critical improvements to repository access, allowing users to manage Docker images across multiple organizations/namespaces from a single workflow. The action now properly handles Docker Hub's namespace model where users can have access to multiple organizations beyond their personal namespace.

## ✨ What's New

### Multi-Namespace Repository Access
- **Cross-Organization Support**: Clean repositories from different organizations in a single action run
- **Flexible Repository Specification**: Use `namespace/repository` format to explicitly target repositories in specific organizations
- **Backward Compatible**: Existing configurations continue to work with the default namespace

### Example Usage
```yaml
repositories: 'myorg/frontend,anotherorg/backend,personal-repo'
```

## 🐛 Bug Fixes

- **Fixed namespace handling**: Resolved issue where the script incorrectly assumed namespace always equals username ([#1](https://github.com/lostlink/docker-cleanup/issues/1))
- **Authentication improvements**: Better handling of authentication tokens for cross-organization access
- **Repository access validation**: Improved error messages when repositories are not found or inaccessible

## 📝 Documentation

- **Enhanced README**: Added clear documentation on namespace handling and multi-organization support
- **Local testing examples**: Updated with namespace-specific repository examples
- **Troubleshooting guide**: Improved guidance for repository access issues

## 🔧 Technical Improvements

- **Repository parsing**: Added robust parsing logic to handle both `repository` and `namespace/repository` formats
- **Test coverage**: Added comprehensive tests for repository specification parsing
- **Code cleanup**: Removed debug artifacts and made codebase ready for public use

## 💥 Breaking Changes

None - This release is fully backward compatible. However, note that:
- Repositories can now be specified as `namespace/repository` for explicit namespace targeting
- The `organization` input now serves as the default namespace for unqualified repository names

## 🚀 Migration Guide

No migration needed. Your existing workflows will continue to work as before. To take advantage of multi-namespace support:

1. **For repositories in your default namespace**: Continue using just the repository name
   ```yaml
   repositories: 'repo1,repo2,repo3'
   ```

2. **For repositories in other organizations**: Use the full namespace/repository format
   ```yaml
   repositories: 'myorg/repo1,anotherorg/repo2,repo3'
   ```

## 📊 Example Configuration

```yaml
- name: Docker Hub Cleanup
  uses: lostlink/docker-cleanup@v1.0.1
  with:
    username: ${{ secrets.DOCKERHUB_USERNAME }}
    password: ${{ secrets.DOCKERHUB_PASSWORD }}
    # Mix repositories from different namespaces
    repositories: 'personal-repo,mycompany/product-api,opensource-org/library'
    pr-retention-days: 30
    sha-retention-days: 14
    dry-run: false
```

## 🙏 Acknowledgments

Thanks to the community for identifying the namespace limitation and providing feedback on multi-organization use cases.

## 📦 Installation

Update your workflow to use the new version:

```yaml
- uses: lostlink/docker-cleanup@v1.0.1
```

Or continue using `@v1` to automatically get minor updates:

```yaml
- uses: lostlink/docker-cleanup@v1
```

## 🐛 Bug Reports

If you encounter any issues, please [report them on GitHub](https://github.com/lostlink/docker-cleanup/issues).

---

**Full Changelog**: [v1.0.0...v1.0.1](https://github.com/lostlink/docker-cleanup/compare/v1.0.0...v1.0.1)