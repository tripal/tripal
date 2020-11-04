![PHPUnit Tests](https://github.com/tripal/tripal/workflows/PHPUnit%20Tests/badge.svg)
[![All Contributors](https://img.shields.io/badge/all_contributors-14-orange.svg?style=flat-square)](#contributors)
[![Documentation Status](https://readthedocs.org/projects/tripal/badge/?version=latest)](https://tripal.readthedocs.io/en/latest/?badge=latest)

[![DOI](https://zenodo.org/badge/42666405.svg)](https://zenodo.org/badge/latestdoi/42666405)


![alt tag](https://raw.githubusercontent.com/tripal/tripal/7.x-3.x/tripal/theme/images/tripal_logo.png)

Welcome to the home of Tripal Development! If you are thinking to yourself "What is Tripal?", head over to [our website, Tripal.info](http://tripal.info/).

# Current Status

 - Recommended Version: ![GitHub release (latest by date)](https://img.shields.io/github/v/release/tripal/tripal?color=brightgreen)
 - Development:
    - [![Tripal 3](https://img.shields.io/badge/dev-7.x--3.x-yellow)](https://github.com/tripal/tripal): Focus is on bug fixes
    - [![Tripal 4](https://img.shields.io/badge/dev-7.x--4.x-yellow)](https://github.com/tripal/t4d8): Full upgrade to Drupal 8/9 (development in [T4D8 Repository](https://github.com/tripal/t4d8)).
 - Tripal ![Tripal 1.x](https://img.shields.io/badge/unsupported-7.x--1.x-red) and ![Tripal 2.x](https://img.shields.io/badge/unsupported-7.x--2.x-red) are no longer supported by the Project Management Committee, although we will accept community submitted fixes for Tripal 2.x.

 # Resources

  - For information on **how to use Tripal** through the Administrative Interface: [Tripal Users Guide](https://tripal.readthedocs.io/en/latest/user_guide.html)
  - For help **extending Tripal** or understanding how it works: [Tripal Developers Guide](https://tripal.readthedocs.io/en/latest/dev_guide.html)
  - To check if another group already developed the functionality you need: [Listing of **Available Extension Modules**](https://tripal.readthedocs.io/en/latest/extensions.html)
  - Tripal is developed by a world-wide community! See the [description of our **Governance Structure**](https://tripal.readthedocs.io/en/latest/contributing/governance.html)
  - If you are the head of a research group looking to fund Tripal Development: [Guide to **Funding Proposal** Development](https://tripal.readthedocs.io/en/latest/contributing/funding.html)
  - For how to **install Tripal**, follow the instructions in the online Tripal User's Guide for [Tripal v2](https://tripal.info/tutorials/v2.x/installation) or [Tripal v3](https://tripal.readthedocs.io/en/latest/user_guide.html).
  - For how to **upgrade from Tripal 2 to Tripal 3**, follow the [Upgrade Instructions](https://tripal.readthedocs.io/en/latest/user_guide/install_tripal/upgrade_from_tripal2.html) in the Tripal v3 User's Guide.

# Contribution

Looking to contribute? That's Amazing -Welcome!!! Here's a quick run-down to get you started:

 - We welcome all contributions!
 - This github issue queue is open to: bug reports, usability concerns, performance issues, site administration or development questions, data storage discussions, etc.
 - We ask that questions/concerns/bugs with **extension modules** be addressed in their respective repositories (including those under the Tripal organization).
 - For **feature request**, please open an issue with detailed use cases and description. We would love to help you design a generic solution for the community whether it belongs in Tripal Core or an Extension module! Please wait to submit a PR until the Project Management Committee (PMC) indicates it is generally applicable and should be included in Core.
 - For **potential bugs or concerns**, please open an issue being as descriptive as possible. Screenshots and steps to reproduce will help us, help you! If you know how to fix the issue, feel free to submit a pull request (PR) linked to the issue!
 - For **documentation** contributions, click the "Edit on Github" link at the top of the page you want to improve! [Instructions for our flavour of ReadtheDocs can be found on ReadtheDocs](https://tripal.readthedocs.io/en/latest/contributing/documentation.html).
 - See our quickstart if you need help on how to submit a PR: [ReadtheDocs, how to create a PR](https://tripal.readthedocs.io/en/latest/contributing/pull_requests.html#how-to-create-a-pr).
 - Click on issues > New Issue for helpful issue templates.
 - For Tripal 4 (Drupal 8/9), see the [T4D8 repository](https://github.com/tripal/t4d8).

For more detailed guidelines see our [full Contribution Documentation](https://tripal.readthedocs.io/en/latest/contributing/pull_requests.html)!


# Github Communication Tips

 - Don’t be afraid to mention people (@username) who are knowledgeable on the topic or invested. We are academics and overcommitted, it’s too easy for issues to go unanswered: don’t give up on us!
 - Likewise, don’t be shy about bumping an issue if no one responds after a few days. Balancing responsibilities is hard.
 - Want to get more involved? Issues marked with “Good beginner issue” are a good place to start if you want to try your hand at submitting a PR.
 - Everyone is encouraged/welcome to comment on the issue queue! Tell us if you
     - are experiencing the same problem
     - have tried a suggested fix
     - know of a potential solution or work-around
     - have an opinion, idea or feedback of any kind!
 - Be kind when interacting with others on Github! (see Code of Conduct below for further guidelines). We want to foster a welcoming, inclusive community!
     - Constructive criticism is welcome and encouraged but should be worded such that it is helpful :-) Direct criticism towards the idea or solution rather than the person and focus on alternatives or improvements.

# Code of Conduct

Be nice! If that’s insufficient, Tripal community defers to https://www.contributor-covenant.org/

# Required Dependencies
* Drupal:
  * Drupal 7.x
  * Drupal core modules: Search, Path and PHP modules.
  * Drupal contributed modules:
    * [Views](http://drupal.org/project/views)
    * [Entity API](http://drupal.org/project/entity)
* PostgreSQL 9.3 or higher (9.5 required for Chado 1.2 to 1.3 upgrade)
* PHP 5.5+
* UNIX/Linux

# Development Testing

To run PHP unit tests on your local system, run `composer install` to install developer-specific requirements.  Next, create a `.env` file in your `/Tests/` directory that defines the `DRUPAL_ROOT` variable, for example

```
DRUPAL_ROOT=/var/www/html
```
Then run PHPUnit from your root Tripal directory.

PHPUnit tests will also be run in the Travis CI build.

Read our [testing guidelines](https://tripal.readthedocs.io/en/latest/contributing/tests.html).

## Contributors

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://github.com/spficklin"><img src="https://avatars0.githubusercontent.com/u/1719352?v=4" width="100px;" alt=""/><br /><sub><b>Stephen Ficklin</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=spficklin" title="Code">💻</a> <a href="#eventOrganizing-spficklin" title="Event Organizing">📋</a> <a href="https://github.com/tripal/tripal/commits?author=spficklin" title="Documentation">📖</a> <a href="#projectManagement-spficklin" title="Project Management">📆</a> <a href="https://github.com/tripal/tripal/pulls?q=is%3Apr+reviewed-by%3Aspficklin" title="Reviewed Pull Requests">👀</a></td>
    <td align="center"><a href="http://www.bradfordcondon.com/"><img src="https://avatars2.githubusercontent.com/u/7063154?v=4" width="100px;" alt=""/><br /><sub><b>Bradford Condon</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=bradfordcondon" title="Code">💻</a> <a href="https://github.com/tripal/tripal/commits?author=bradfordcondon" title="Documentation">📖</a> <a href="#projectManagement-bradfordcondon" title="Project Management">📆</a> <a href="#eventOrganizing-bradfordcondon" title="Event Organizing">📋</a> <a href="https://github.com/tripal/tripal/pulls?q=is%3Apr+reviewed-by%3Abradfordcondon" title="Reviewed Pull Requests">👀</a></td>
    <td align="center"><a href="https://laceysanderson.github.io/"><img src="https://avatars3.githubusercontent.com/u/1566301?v=4" width="100px;" alt=""/><br /><sub><b>Lacey-Anne Sanderson</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=laceysanderson" title="Code">💻</a> <a href="https://github.com/tripal/tripal/commits?author=laceysanderson" title="Documentation">📖</a> <a href="#projectManagement-laceysanderson" title="Project Management">📆</a> <a href="#eventOrganizing-laceysanderson" title="Event Organizing">📋</a> <a href="https://github.com/tripal/tripal/pulls?q=is%3Apr+reviewed-by%3Alaceysanderson" title="Reviewed Pull Requests">👀</a></td>
    <td align="center"><a href="https://github.com/chunhuaicheng"><img src="https://avatars2.githubusercontent.com/u/14333886?v=4" width="100px;" alt=""/><br /><sub><b>chunhuaicheng</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=chunhuaicheng" title="Code">💻</a></td>
    <td align="center"><a href="https://github.com/shawnawsu"><img src="https://avatars1.githubusercontent.com/u/24374002?v=4" width="100px;" alt=""/><br /><sub><b>Shawna</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=shawnawsu" title="Code">💻</a> <a href="#content-shawnawsu" title="Content">🖋</a> <a href="https://github.com/tripal/tripal/commits?author=shawnawsu" title="Documentation">📖</a> <a href="https://github.com/tripal/tripal/pulls?q=is%3Apr+reviewed-by%3Ashawnawsu" title="Reviewed Pull Requests">👀</a></td>
    <td align="center"><a href="https://github.com/mboudet"><img src="https://avatars0.githubusercontent.com/u/17642511?v=4" width="100px;" alt=""/><br /><sub><b>mboudet</b></sub></a><br /><a href="https://github.com/tripal/tripal/issues?q=author%3Amboudet" title="Bug reports">🐛</a></td>
    <td align="center"><a href="https://github.com/guignonv"><img src="https://avatars1.githubusercontent.com/u/7290244?v=4" width="100px;" alt=""/><br /><sub><b>Valentin Guignon</b></sub></a><br /><a href="https://github.com/tripal/tripal/issues?q=author%3Aguignonv" title="Bug reports">🐛</a></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/mestato"><img src="https://avatars1.githubusercontent.com/u/508122?v=4" width="100px;" alt=""/><br /><sub><b>Meg Staton</b></sub></a><br /><a href="#fundingFinding-mestato" title="Funding Finding">🔍</a> <a href="#eventOrganizing-mestato" title="Event Organizing">📋</a></td>
    <td align="center"><a href="https://github.com/abretaud"><img src="https://avatars3.githubusercontent.com/u/238755?v=4" width="100px;" alt=""/><br /><sub><b>Anthony Bretaudeau</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=abretaud" title="Code">💻</a></td>
    <td align="center"><a href="https://github.com/colthom"><img src="https://avatars0.githubusercontent.com/u/17720870?v=4" width="100px;" alt=""/><br /><sub><b>colthom</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=colthom" title="Documentation">📖</a></td>
    <td align="center"><a href="http://almsaeedstudio.com"><img src="https://avatars2.githubusercontent.com/u/1512664?v=4" width="100px;" alt=""/><br /><sub><b>Abdullah Almsaeed</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=almasaeed2010" title="Code">💻</a> <a href="https://github.com/tripal/tripal/pulls?q=is%3Apr+reviewed-by%3Aalmasaeed2010" title="Reviewed Pull Requests">👀</a></td>
    <td align="center"><a href="https://github.com/btski"><img src="https://avatars1.githubusercontent.com/u/32686196?v=4" width="100px;" alt=""/><br /><sub><b>btski</b></sub></a><br /><a href="#question-btski" title="Answering Questions">💬</a></td>
    <td align="center"><a href="https://github.com/ekcannon"><img src="https://avatars0.githubusercontent.com/u/3409057?v=4" width="100px;" alt=""/><br /><sub><b>ekcannon</b></sub></a><br /><a href="#ideas-ekcannon" title="Ideas, Planning, & Feedback">🤔</a> <a href="#eventOrganizing-ekcannon" title="Event Organizing">📋</a></td>
    <td align="center"><a href="https://github.com/jlwegrzyn"><img src="https://avatars1.githubusercontent.com/u/50996590?v=4" width="100px;" alt=""/><br /><sub><b>jlwegrzyn</b></sub></a><br /><a href="#fundingFinding-jlwegrzyn" title="Funding Finding">🔍</a></td>
  </tr>
  <tr>
    <td align="center"><a href="http://www.vcru.wisc.edu/simonlab/sdata/software/index.html"><img src="https://avatars3.githubusercontent.com/u/8419404?v=4" width="100px;" alt=""/><br /><sub><b>Douglas Senalik</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=dsenalik" title="Code">💻</a></td>
    <td align="center"><a href="https://github.com/par12005"><img src="https://avatars0.githubusercontent.com/u/38262122?v=4" width="100px;" alt=""/><br /><sub><b>par12005</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=par12005" title="Code">💻</a> <a href="#question-par12005" title="Answering Questions">💬</a></td>
    <td align="center"><a href="http://sofiarobb.com"><img src="https://avatars0.githubusercontent.com/u/1072991?v=4" width="100px;" alt=""/><br /><sub><b>Sofia Robb</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=srobb1" title="Code">💻</a></td>
    <td align="center"><a href="https://github.com/katherynbuble"><img src="https://avatars3.githubusercontent.com/u/59714566?v=4" width="100px;" alt=""/><br /><sub><b>katherynbuble</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=katherynbuble" title="Code">💻</a></td>
    <td align="center"><a href="https://github.com/risharde"><img src="https://avatars3.githubusercontent.com/u/33866403?v=4" width="100px;" alt=""/><br /><sub><b>risharde</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=risharde" title="Code">💻</a></td>
    <td align="center"><a href="https://github.com/pgrimaud"><img src="https://avatars1.githubusercontent.com/u/1866496?v=4" width="100px;" alt=""/><br /><sub><b>Pierre Grimaud</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=pgrimaud" title="Documentation">📖</a></td>
  </tr>
</table>

<!-- markdownlint-enable -->
<!-- prettier-ignore-end -->
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
