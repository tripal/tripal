# Guidelines for Contribution to Tripal

The following guidelines are meant to encourage contribution to Tripal source-code on GitHub by making the process open, transparent and collaborative. If you have any feedback including suggestions for improvement or constructive criticism, please [comment on the github issue](https://github.com/tripal/tripal/issues/344). **These guidelines apply to everyone contributing to Tripal whether it's your first time (Welcome!) or project management committee members.**

**Note:** _These guidelines are specifically for contributing to_ [_https://github.com/tripal/tripal_](https://github.com/tripal/tripal)_. However, we encourage all Tripal extension modules to consider following these guidelines to foster collaboration among the greater Tripal Community._

Guidelines serve as suggestions ( **should** ) or requirements (**must).** _When the word "should" is used in the text below, the stated policy is expected but there may be minor exceptions.  When the word "must" is used there are no exceptions to the stated policy._

## Github Communication Tips

- Don't be afraid to mention people (@username) who are knowledgeable on the topic or invested.*- We are academics and overcommitted, it's too easy for issues to go unanswered.
  - Likewise, don't be shy about bumping an issue if no one responds after a few days.  Balancing responsibilities is hard.
- Want to get more involved? Issues marked with "Good beginner issue" are a good place to start if you want to try your hand at submitting a PR.
- Everyone is encouraged/welcome to comment on the issue queue!** Tell us if you
    - are experiencing the same problem
    - have tried a suggested fix
    - know of a potential solution or work-around
    - have an opinion, idea or feedback of any kind!
- Be kind when interacting with others on github!** (see Code of Conduct below for further guidelines). We want to foster a welcoming, inclusive community!
    - Constructive criticism is welcome and encouraged but should be worded such that it is helpful :-) Direct criticism towards the idea or solution rather than the person and focus on alternatives or improvements.

## Bugs

- Every bug **should** be reported as a github issue.
  - Even if a bug is found by a committer who intends to fix it themselves immediately, they **should** create an issue and assign it to themselves to show their intent.
- Please follow the issue templates as best you can.  This information makes discussion easier and helps us resolve the problem faster.
  - Also provide as much information as possible :-)  Screenshots or links to the issue on a development site can go a long way!
- Bonus points for unit tests to ensure the bug does not return :-)

## Feature Requests

- Every feature request should start as an issue so that discussion is encouraged :-)**
- Please provide the following information (bold is required; underlined strengthens your argument):
    - **Use Case:** fully describe why you need/want this feature
    - Generally Applicable: Why do you feel this is generally applicable? Suggest other use cases if possible. Mention (@) others that might want/need this feature.
    - Implementation: Describe a possible implementation. Bonus points for configuration, use of ontologies, ease of use, permission control, security considerations
- All features **should** be optional so that Tripal admin can choose to make it available to their users.
    - When applicable, new features should be designed such that tripal-site admin can disable them.
    - Bonus points: for making new features configurable and easily themed.
- Feature requests will be voted on by the project management and advisory/steering committees to determine if it should be included in core, an existing extension module or it's own extension module.
    - Votes should be based on whether this feature is generally applicable and doesn't exclude existing users and not be biased by the needs of your own Tripal site.
- If a feature isn't suitable for inclusion within Tripal core, use the issue discussion as a springboard to create a Tripal extension module!

**Note:** _In the future there will be a set of guidelines for what should be included in core. This will make the process of requesting new features more streamlined, inclusive and transparent._

## Pull Request (PR) Guideline

The goal of this document is to make it easy for **A)** contributors to make pull requests that will be accepted, and **B)** Tripal committers to determine if a pull request should be accepted.

- PRs that address a specific issue **must** link to the related issue page.
  - Really in almost every case, there should be an issue for a PR.  This allows feedback and discussion before the coding happens.  Not grounds to reject, but encourage users to create issues at start of their PR.  Better late than never :).
- Each PR **must** be tested/approved by at least 2 users with at least one user being a "trusted committer."
  - Testers **should** describe how the testing was performed if applicable (allows others to replicate the test).
  - Tripal's guiding philosophy is to encourage open contribution.  With this in mind, committers should **work with contributors** to resolve issues in their PRs.  PRs that will not be merged should be closed, **transparently citing** the reason for closure.  In an ideal world, features that would be closed are discouraged at the **issue phase** before the code is written!
  - The pull request branch should be deleted after merging (if not from a forked repository) by the person who performs the merge.
- PRs that include new functionality **must** also provide Unit Tests.
  - Tests **must** test the new functionality added.
  - Bonus points for testing all surrounding functionality.
    - _Note: testing surrounding functionality is highly encouraged if the submitter is on the PMC ;-)_
  - For example, when adding feature X to custom tables, the PR must include tests for feature X and we would be greatly appreciative if it included tests for custom tables in general :-D.
- PRs **should** pass all Travis-CI tests before they are merged.
- Branches **should** follow the following format:
    - [issue\_number]-[tripal\_version]-[short\_description]
  - tripal\_version being Tv2, Tv3, etc.
  - "-[short\_description]" being optional but highly encouraged
- **Must** follow Drupal code standards: [https://www.drupal.org/docs/develop/standards](https://www.drupal.org/docs/develop/standards)
- PRs for new feature should remain open until adequately discussed (see guidelines below) and approved by a vote (all members of the PMC must vote in favour).

**Note:** _See the Tripal PR Tutorial for more guidance on how to actually create a PR if contribution via github is new to you (_ [_KnowPulse workflow for inspiration_](https://github.com/UofS-Pulse-Binfo/KnowPulse/blob/master/Workflow.md)_)_

## General Project Management

- **Every task related to Tripal should be in github, either as it's own issue or grouped with like tasks into a single issue.** This effectively puts our todo list on github making it transparent to anyone who wants to help. It has the benefit of showing how active our community is, keeps everyone informed with where Tripal is headed and makes it easy for others to chime in with experience, comments and support.
- **Guidelines for Tagging Issues:**
    - The first committer who comments on an issue should tag it with the version of Tripal it applies to.
    - Issues with a suggested fix or work-around should be tagged with "Fix Required" to let others know a PR is needed.
    - Only tag an issue with "bug" once it has been shown to be reproducible. If it's not reproducible by a committer but you feel it is a bug then tag it as "potential bug".
    - If multiple users have commented that a bug affects them, tag it as "affects multiple users".
    - Issues that require a PR and someone with relatively little Tripal experience could fix should be tagged with "Good beginner issue"
    - All feature requests should be tagged as an "enhancement"
    - If you are the first reviewer to confirm a PR works, tag it with "Reviewer #1 Approval"
- **Guidelines for Discussion:**
    - No requirement for discussion (still requires 2 reviews): Minor bug fixes, changes to inline comments, addition of unit tests, minor code typos
    - Requires Discussion: Major changes, new features, and issue at the discretion of the PMC
      - Add the "discussion" tag to any issue requiring discussion
      - Discussion Tag is removed when adequate discussion has taken place (at the discretion of the person who added the tag)
      - Additionally, new features require that all members of the PMC have had a chance to contribute to the discussion and feel satisfied.
- Please use the **assignment** feature to clarify who will be contributing the code to prevent duplication of effort.
    - When assigning yourself, comment on what your timeline is. This allows others to jump in if they have time sooner.
    - If you would like to **take over a PR assigned to someone else** , comment asking for an update and offer your services.
    - If the author of the issue plans on contributing the fix themselves but is not a committer, they should indicate that in the issue.  A committer will assign them the issue.
- When you start working on an issue, you **should** create the branch and push to it regularly. If you are working on a fork, you're **encouraged** to link to it in the issue.
    - Committers can work on a fork or directly.  If the branch is on tripal/tripal, then other committers should contribute via PR unless otherwise agreed
- If an issue is identified as being relevant to another repository (ie a tripal module, not core), a new issue **should** be created, cross referenced, and the original issue should be closed encouraging discussion in the module.

## Code of Conduct

- Be nice!  If that's insufficient, Tripal community defers to https://www.contributor-covenant.org/

## Testing/CI

Exhaustive guides to testing are available at tripal.info (here for now: [https://www.bradfordcondon.com/2018/05/02/tripal\_testing\_guidelines/](https://www.bradfordcondon.com/2018/05/02/tripal_testing_guidelines/)).  Below are guiding principles.

- All tests pass.
- Tests don't modify db: use transactions and factories.
- Tests are organized properly: by submodule and function.
- Tests run quietly.

## Changes to this Document

These guidelines are binding to the Tripal Community. If you have comments, suggestions or constructive criticism please bring it up in a [comment on the github issue](https://github.com/tripal/tripal/issues/344). Changes to this document will be made after adequate discussion has occurred and the project management committee has voted in favour of the change.

# Tripal Governance

The above document makes us a sort of hybrid between a [meritocracy and liberal contribution model](https://opensource.guide/leadership-and-governance/#what-are-some-of-the-common-governance-structures-for-open-source-projects).


The Tripal project recognizes these roles:

- Users - They have downloaded Tripal!  Maybe they even have a site!
- Contributors - contributing!  Code, comments, discussion, questions, bug reports.
- Committers - write access to the repository
- PMC - Makes **code relevant** decisions.
- Oversight committee - Makes **policy leve** decisions.  This may overlap with PMC, but the idea is the oversight committee includes parties who are **not necessarily coders** and therefore not reviewing Pull requests etc.

More guidelines coming soon.
