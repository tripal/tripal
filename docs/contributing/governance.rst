Tripal Governance
==================

We wish to maintain Tripal as an open source project and therefore want to empower  Tripal adopters and developers as much as possible in the development of Tripal, while keeping the project coherent, focused, and useable to a wide range of adopters. As the Tripal community grows, it is prudent to set up a formal governance model. This document describes this model.

The Tripal project recognizes these roles:
 - **End-Users**: They are users of Tripal-based websites (not developers of a site).
 - **Adopters**: They have downloaded Tripal. Maybe they even have a site!
 - **Extension Contributors**: they extend Tripal through modules, themes, views, data loaders, fields, and/or libraries.
 - **Core Code Contributors**: contribute code to the `Tripal "core" <https://github.com/tripal/tripal>`_, comments, discussion, questions, bug reports.
 - **Core Code Committers**: write access to the `Tripal "core" repository <https://github.com/tripal/tripal>`_.
 - **Tripal Project Management Committee (PMC)**: make *code relevant* decisions, (i.e. ensure code standards, robustness, and long-term design objectives are maintained).
 - **Tripal Advisory Committee (TAC)**: Provide guidance to the PMC for *policy-level* and *future planning* recommendations.

Adopters
----------

Any person who wishes to or has downloaded/set up a Tripal site.  Everyone in this group is invited to the monthly user meetings and is encouraged to ask questions and make suggestions.

Extension Contributors
-----------------------

These are developers who are extending Tripal. Extension contributors are encouraged to make their extensions available on the `Tripal GitHub organization <https://github.com/tripal>`_ and list them on the `Tripal Documentation <https://tripal.readthedocs.io/en/latest/extensions.html>`_. Extension contributors are also encouraged to use the `Tripal Module Rating System <https://tripal.readthedocs.io/en/latest/extensions/module_rating.html>`_ as a guideline for developing high quality modules, which are easier to adopt by the greater Tripal community.

Core Code Contributors
------------------------
Core Code Contributors are active members of the Tripal community who make suggestions to improve and/or contribute code to Tripal Core. Core Code Contributors are encouraged to submit pull requests to the Tripal core and attend monthly user calls. For more information, see the `Guidelines for Contributing to Tripal core <https://tripal.readthedocs.io/en/latest/contributing/pull_requests.html>`_.

Responsibilities include:
 - Monitor Tripal core issue queue.
 - Submit pull requests.

Committers
------------

These are dedicated Tripal developers who are trusted to commit pull requests directly to the Tripal core repository. They are encouraged to be active in the Tripal community and routinely review pull requests. Developers are added to to committers group by unanimous agreement from the PMC.

Responsibilities include:
 - Monitor Tripal core issue queue.
 - Review and merge pull requests.

See the `guidelines for contributors <https://tripal.readthedocs.io/en/latest/contributing/pull_requests.html>`_ for more details.

The Tripal Project Management Committee (PMC)
------------------------------------------------

This group consists of experienced Tripal developers.

Responsibilities include:
 - Ensure good practices, for example, submitting errors, questions, and requests via GitHub.
 - Monitor issue queue (though this responsibility isn't limited to the PMC).
 - Resolve questions and differences of opinions among Contributors.
 - Work with the TAC to make decisions about significant new features. Examples:
     - a new core module,
     - designing a module-specific REST API,
     - new technologies or libraries used by the core code.
 - Avoid feature bloat; judge what contributions benefit many versus those that are specific to the needs of the contributor.
 - Final approval of submitting guidelines (see `guidelines for contribution <https://tripal.readthedocs.io/en/latest/contributing/pull_requests.html>`_).
 - Set coding standards.
 - Ensure Tripal remains generic and useful to a wide range of groups.

The PMC will strive to obtain consensus, and all members ensure that the Tripal community and the TAC are informed on decisions. Any individual member can call a meeting. The term will be two years with the possibility of extension. At least one member will serve on the TAC; this person will be elected by vote within the PMC.

Communication and Meetings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The PMC will meet as necessary. It is expected that frequent decisions will need to be made, some through GitHub issue comments, Slack, e-mail, or conference calls.

Tripal Advisory Committee (TAC)
---------------------------------
The Tripal Advisory Committee (TAC) provides leadership, guidance, future planning, advocacy and coordination for Tripal. The TAC acts in an advisory capacity only by determining sets of recommendations for Tripal. All recommendations will be provided to the PMC. Topics include recommended technologies, overall approach, software architecture, development priorities, timelines, funding strategies, best practices, support for a fair and focused open source development strategy.

At least one member of the PMC must be on the TAC to ensure that the reality of what is and is not feasible for the developers is not lost. Additionally, close communication between the TAC and PMC is critical, as is transparency of the TAC discussions to the entire Tripal community. All members of the PMC are welcome at TAC meetings.

Membership
^^^^^^^^^^^^
The TAC should include "internal" and "external" members. Internal members are individuals who manage Tripal websites or lead teams engaged in active development, possibly with funding to do so. External members may be outside the Tripal community altogether, and may include government, non-profit, or industry representatives who are stakeholders for one or more Tripal databases (but not active managers of a Tripal site) and/or specialists in such disciplines as cyberinfrastructure, bioinformatics, genomics, breeding.

- Terms are for two years.
- Two year memberships can be renewed for individuals who wish to stay on to complete a particular objective.
- Membership is capped at 15.
   - Initial Setup:
      - Start small and move larger as needed.
      - Set minimum sizes for number of internal and external members.
      - Committee should be organized before inviting external members.
      - Stagger ends of terms to ensure continuity.
- The minimum number of internal members is 3.
- The number of internal members should not be less than 1/2.
- The target number of external members is 5.
- If the TAC decides to replace a leaving member, the current members will develop a list of possible candidates. The chair will contact each in turn until the membership slot is filled.
- Members will be asked to serve by the current TAC.

Responsibilities include:
 - Serving a minimum two year term, beginning with the yearly board meeting (see below) in conjunction with the January Plant and Animal Genome Conference in San Diego.
 - Respond to issues in a timely manner when contacted directly. Members are strongly encouraged to become part of the TAC GitHub group, and if they wish to comment or discuss agenda items directly with the community, to do so in the GitHub issue queue (instead of the email list serve).
 - Attend the annual January meeting at PAG and at least three of the quarterly meetings.
 - Review agenda and supporting materials prior to meetings.
 - Stay informed about Tripal, its member databases, developers, and users.

In addition, internal members are responsible for:
 - Actively communicating with the Tripal community, both to collect ideas and concerns and to inform the community about TAC plans for Tripal.
 - Engaging in the Tripal Core GitHub Issue queue on “discussion” issues.

TAC Chair
^^^^^^^^^^^

The board will be led by a chair to be elected by TAC members at the January meeting annually (see below). One or more vice-chairs can be designated by the chair. The chair will ensure that the following is accomplished, delegating responsibilities as needed:
 - Organize, announce and lead TAC meetings.
 - Write the meeting agenda and post to Tripal.info.
 - Provide supporting materials for review at least 1 week before TAC meetings.
 - Ensure that the agenda items that would benefit from review by the community are posted to the GitHub Tripal core issue queue. Ensure that any GitHub issue discussions are linked on the agenda and available for review by the TAC.
 - Ensure meeting notes are taken by someone present at the meeting and posted to Tripal.info.
 - Call for votes on TAC recommendations when community voting is required.
 - Call additional meetings if needed.
 - Facilitate communication between the TAC and PMC.
 - Filling vacant slots on the TAC.
 - The chair has voting privileges.

TAC Meeting Agenda Items
^^^^^^^^^^^^^^^^^^^^^^^^^^

Strongly encouraged to be posted to the GitHub Tripal core issue queue as well as to tripal.info, to inform and solicit community comment. TAC meeting agendas will include issues tagged “TAC Next Meeting” on the GitHub Tripal core issue queue. Other agenda items may be added by the TAC chair or members, or by the PMC. These issues will be closed after the meeting.

Communication and Meetings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The primary TAC meeting will be held in January of each year, at the Tripal codefest. In-person attendance is strongly encouraged, but a teleconference option will be provided. Each issue on the agenda will be discussed, and if needed, the chair will call for a vote to determine the final recommendation of the TAC. Votes carry based on simple majority. All discussion, votes and objections will be recorded in meeting notes, which will be posted on Tripal.info.

Additional teleconference TAC meetings will be held once per quarter (April, July, October).  These could be held in place of the monthly Tripal User's Meeting to avoid meeting overload.

TAC meetings outside the above schedule may be called by the TAC chair. These will only be called in urgent situations. In less urgent situations, the TAC chair or the TPMC can contact the internal members of the TAC and request a meeting or solicit comments via email, GitHub issue, or Slack.

At any time the TPMC may communicate with members of the TAC with expertise in specific areas if needed to assist in decision making.

Changes to this Document
--------------------------

These guidelines define the structure for official Tripal Management and Governance. If you have comments or questions, please `post them as a Github issue <https://github.com/tripal/tripal/issues/new?template=discussion.md>`_ or ask them at the user's meeting. Changes to this document will be made after adequate discussion has occurred and the project management committee has voted in favor of the change.
