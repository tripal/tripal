Guidelines for Contribution to Tripal
========================================

The following guidelines are meant to encourage contribution to Tripal source-code on GitHub by making the process open, transparent and collaborative. If you have any feedback including suggestions for improvement or constructive criticism, please `comment on the Github issue <https://github.com/tripal/tripal/issues/344>`_. **These guidelines apply to everyone contributing to Tripal whether it's your first time (Welcome!) or project management committee members.**

.. note::

  These guidelines are specifically for contributing to `Tripal <https://github.com/tripal/tripal>`_. However, we encourage all Tripal extension modules to consider following these guidelines to foster collaboration among the greater Tripal Community.

.. note::

	Guidelines serve as suggestions ( **should** ) or requirements (**must**). When the word "should" is used in the text below, the stated policy is expected but there may be minor exceptions.  When the word "must" is used there are no exceptions to the stated policy.


Github Communication Tips
---------------------------

- Don't be afraid to mention people (@username) who are knowledgeable on the topic or invested.  *We are academics and overcommitted, it's too easy for issues to go unanswered: don't give up on us!*
- Likewise, don't be shy about bumping an issue if no one responds after a few days. *Balancing responsibilities is hard.*
- Want to get more involved? Issues marked with "Good beginner issue" are a good place to start if you want to try your hand at submitting a PR.
- Everyone is encouraged/welcome to comment on the issue queue! Tell us if you
    - are experiencing the same problem
    - have tried a suggested fix
    - know of a potential solution or work-around
    - have an opinion, idea or feedback of any kind!
- Be kind when interacting with others on Github! (see Code of Conduct below for further guidelines). We want to foster a welcoming, inclusive community!
    - Constructive criticism is welcome and encouraged but should be worded such that it is helpful :-) Direct criticism towards the idea or solution rather than the person and focus on alternatives or improvements.

Bugs
-----


- Every bug **should** be reported as a Github issue.
    - Even if a bug is found by a committer who intends to fix it themselves immediately, they **should** create an issue and assign it to themselves to show their intent.
- Please follow the issue templates as best you can.  This information makes discussion easier and helps us resolve the problem faster.
    - Also provide as much information as possible :-)  Screenshots or links to the issue on a development site can go a long way!
- Bonus points for unit tests to ensure the bug does not return :-)

Feature Requests
------------------

- Every feature request should start as an issue so that discussion is encouraged :-)
- Please provide the following information (bold is required; underlined strengthens your argument):
    - **Use Case:** fully describe why you need/want this feature
    - Generally Applicable: Why do you feel this is generally applicable? Suggest other use cases if possible. Mention (@) others that might want/need this feature.
    - Implementation: Describe a possible implementation. Bonus points for configuration, use of ontologies, ease of use, permission control, security considerations
- All features **should** be optional so that Tripal admin can choose to make it available to their users.
    - When applicable, new features should be designed such that Tripal-site admin can disable them.
    - Bonus points: for making new features configurable and easily themed.
- Feature requests will be voted on by the project management and advisory/steering committees to determine if it should be included in core, an existing extension module or it's own extension module.
    - Votes should be based on whether this feature is generally applicable and doesn't exclude existing users and not be biased by the needs of your own Tripal site.
- If a feature isn't suitable for inclusion within Tripal core, use the issue discussion as a springboard to create a Tripal extension module!

.. note::

  In the future there will be a set of guidelines for what should be included in core. This will make the process of requesting new features more streamlined, inclusive and transparent.

Pull Request (PR) Guideline
----------------------------

The goal of this document is to make it easy for **A)** contributors to make pull requests that will be accepted, and **B)** Tripal committers to determine if a pull request should be accepted.

- PRs that address a specific issue **must** link to the related issue page.
    - In almost every case, there should be an issue for a PR.  This allows feedback and discussion before the coding happens.  Not grounds to reject, but encourage users to create issues at start of their PR.  Better late than never :).
- Each PR **must** be tested/approved by at least 1 contributor, if approved, a "trusted committer" will merge the PR.
    - Testers **should** describe how the testing was performed if applicable (allows others to replicate the test).
    - While Tripal's review body is small, the code review must be a thorough functional test.
    - At the Project Management Committee's (PMC) discretion, a PR may be subject to a non-functional review.  Generally these are small and obvious commits.
    - Tripal's guiding philosophy is to encourage open contribution.  With this in mind, committers should **work with contributors** to resolve issues in their PRs.  PRs that will not be merged should be closed, **transparently citing** the reason for closure.  In an ideal world, features that would be closed are discouraged at the **issue phase** before the code is written!
    - The pull request branch should be deleted after merging (if not from a forked repository) by the person who performs the merge.
- PRs that include new functionality **must** also provide Unit Tests.
    - Tests **must** test the new functionality added.
    - Bonus points for testing all surrounding functionality.
    - For example, when adding feature X to custom tables, the PR must include tests for feature X and we would be greatly appreciative if it included tests for custom tables in general :-D.
- PRs **should** pass all Travis-CI tests before they are merged.
- Branches **should** follow the following format:
    - ``[issue\_number]-[tripal\_version]-[short\_description]``
    - ``tripal\_version`` being Tv2, Tv3, etc.
    - ``-[short\_description]`` being optional but highly encouraged
- **Must** follow `Drupal code standards <https://www.drupal.org/docs/develop/standards>`_
- PRs for new feature should remain open until adequately discussed (see guidelines below) and approved by a vote (all members of the PMC must vote in favour).


How to create a PR
^^^^^^^^^^^^^^^^^^^^^

There are great instructions on creating a PR on `Digital Ocean: How To Create a Pull Request on GitHub <https://www.digitalocean.com/community/tutorials/how-to-create-a-pull-request-on-github>`_.

**The tl;dr version:**

1. `Fork the repository <https://docs.github.com/en/github/getting-started-with-github/fork-a-repo>`_ or `update an existing fork <https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/syncing-a-fork>`_
2. `Clone <https://docs.github.com/en/github/creating-cloning-and-archiving-repositories/cloning-a-repository>`_ the fork
3. `Create a branch <https://git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging>`_ specific to your change: ``[issue\_number]-[tripal\_version]-[short\_description]``
4. Make your changes, `committing <https://git-scm.com/docs/git-commit#_examples>`_ often with useful commit messages.
5. `Push <https://git-scm.com/docs/git-push#_examples>`_ your changes to your fork.
6. `Create a PR by going to your fork <https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request-from-a-fork>`_: target should be ``tripal:7.x-3.x``. For specifics, see guidelines above.

.. note:

  If you are a `committer <>`_, you can clone the Tripal repository directly with no need to create or maintain a fork. Please make sure you are always creating new branches off of ``7.x-3.x`` and that you have pulled all recent changes to ``7.x-3.x`` before creating a new branch.

How PRs and Issues are Handled
------------------------------
The Project Management Committee (PMC) and trusted committers will follow specific rules when working with all issues and pull requests. The rules are listed below. Anyone may provide bug fixes in which case some of the following will apply to all:

- **Every task related to Tripal (bug, feature requests, documentation, discussions) should be in Github, either as it's own issue or grouped with like tasks into a single issue.** This effectively puts our todo list on github making it transparent to anyone who wants to help. It has the benefit of showing how active our community is, keeps everyone informed with where Tripal is headed and makes it easy for others to chime in with experience, comments and support.
- **Guidelines for Tagging Issues:**
    - The first committer who comments on an issue should tag it with the version of Tripal it applies to.
    - Issues with a suggested fix or work-around should be tagged with "Fix Required" to let others know a PR is needed.
    - Only tag an issue with "bug" once it has been shown to be reproducible. If it's not reproducible by a committer but you feel it is a bug then tag it as "potential bug".
    - If multiple users have commented that a bug affects them, tag it as "affects multiple users".
    - Issues that require a PR and someone with relatively little Tripal experience could fix should be tagged with "Good beginner issue"
    - All feature requests should be tagged as an "enhancement"
    - If you are the first reviewer to confirm a PR works, tag it with "Reviewer #1 Approval"
- **Guidelines for Discussion:**
    - Issues that do not require discussion (PRs still require 2 reviews): minor bug fixes, changes to inline comments, addition of unit tests, minor code typos
    - Issues that require discussion: major changes, new features, and issue at the discretion of the PMC
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

Code of Conduct
----------------


- Be nice!  If that's insufficient, Tripal community defers to https://www.contributor-covenant.org/

Testing/CI
------------


Comprehensive guides to testing are available in the :ref:`tests` section.  Below are guiding principles.

- All tests pass.
- Tests don't modify db: use transactions and factories.
- Tests are organized properly: by submodule and function.
- Tests run quietly.
