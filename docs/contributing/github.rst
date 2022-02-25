
Shared Repository Management
=============================

This section describes the current guidelines for committing to the shared Tripal repository. These rules help keep the repository an organized place to work.

Branch Naming Conventions
--------------------------

``tv4g[0-9]-issue\d+-[optional short descriptor]``

Where,
 - ``tv4g[0-9]`` indicates the roadmap group the branch relates to. You can see `the listing of groups here <https://github.com/tripal/t4d8/labels?q=GROUP>`_.
 - ``issue\d+``  indicates the issue describing the purpose of the branch. By making a new issue for each major task before we start working on it, we give room for others to jump in and save you time if something is already done, beyond scope, or can be made easier by something they are working on!
 - ``[optional short descriptor]`` can be anything without spaces. This is meant to make the branches more readable so we don't have to look up the issue every time. You are encouraged to **only have one branch per issue!** That said, there are some edge-cases where multiple branches may be needed (i.e. partitioned reviews) where variations in the optional short description can make the purpose of multiple branches clear.

Outdated Branches
------------------

**It is important to only keep branches that you are actively working on.**

Branches should be deleted as soon as a pull request (PR) has merged them into the main branch.

Unable to Finish
^^^^^^^^^^^^^^^^^^

If you simply **do not have time to finish** the issue associated with a branch, please do the following:

1. Communicate that in the issue. We all understand that academia is a constantly shifting world of priorities! There is no need to justify the change but we do really appreciate the following:

	- describing where you are currently at progress-wise
	- any difficulties or concerns to ran up against
	- what your plan or design were

2. Make sure all your code is committed, pushed to GitHub and add the last commit hash to your description in the above step. This allows us to recover your branch in case anything goes wrong with the patch created in the next step.

3. Create a patch capturing your current progress and attach that to the corresponding issue. See "How to Save Progress from a Branch" below for detailed instructions.

4. Unless someone has said they will take over the work within the next week, please delete the associated branch.

Other Reasons for Abandoning a Branch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you find that you have to **switch gears, try a new approach or otherwise abandon** an open branch, please do the following:

1. Make sure all your code is committed, pushed to GitHub and add the last commit hash to the issue associated with the branch. This allows us to recover your branch in case anything goes wrong with the patch created in the next step.

2. Create a patch capturing the changes in that branch and attach the patch file to the associated issue for the branch. See "How to Save Progress from a Branch" below for detailed instructions.

3. Include your reasoning for abandoning the branch in the issue associated with it. This does not have to be long but should be enough for anyone to understand your reasoning without needing to talk to you.

4. Delete the branch.

How to Save Progress from a Branch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Make sure all your code is committed and pushed to GitHub.
2. Go to our repository on GitHub and select your branch from the drop-down list on the left side to switch to that branch.

	.. image:: github.saveprogress.1.png

3. Click on "Contribute" on the right side and then "Compare".

	.. image:: github.saveprogress.2.png

4. Add ``.patch`` to the end of the URL...

	.. image:: github.saveprogress.3.png

  and click enter to see a text patch of all the changes. This will include any new files as well as smaller changes within existing files. Furthermore, it includes attribution to you to ensure that if this work is used in the future, you get credit for it.

	.. image:: github.saveprogress.4.png

5. Use your browser to save this page as a file and attach it to the issue by dragging it into the text area.


How to Recover an Previously Deleted Branch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Checkout the main branch and pull all changes.

2. Checkout the last commit made in the branch to be recovered. This commit hash should be recorded in the issue comments from when the branch was deleted.

	``git checkout COMMITHASH``

3. Create a new branch pointing to that commit. This branch should follow the naming conventions above.

	``git branch RECOVERED-BRANCH-NAME``

4. Push the new branch to GitHub and carry on like the branch was never deleted!
