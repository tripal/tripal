Galaxy Integration
===============

The Tripal Galaxy module is designed to support integration of Tripal with Galaxy.  In the past, community databases have often provided analytical tools that come prepared with site-specific data.  Examples include BLAST, CAP3, and InterProScan servers, for example.  These tools eased the informatic burden for some researchers by providing tools with site-specific data in easy to use web interfaces.  With larger datasets and more complicated analytical workflows creating online tools becomes a more challenging task.

By integrating Tripal with Galaxy, the Tripal-based community database can offer more complicated analytical tools that support larger data sets using Galaxy as a backend.  To this end, analytical workflows are created by site developers or bioinformaticists inside of the Galaxy user interface.  Once tested and ready, the Tripal Galaxy module communicates with the Galaxy server to provide a web front-end for that workflow.  Users can execute the workflow within the Tripal site, providing a user interface that site-users are familiar and comfortable with.  Users need not know how to use Galaxy to execute the workflow, although, attribution is appropriately provided to the Galaxy server that provides the computation.

The Tripal Galaxy module provides more than just a "wrapper" for Galaxy.  Site administrators can provide files to help end-users easily integrate data from the site within workflows. On Tripal v3 sites, user's can create data collection containing data gleaned from the site which in turn can be used in Galaxy workflows.  Quotas are provided to prevent users from overunning the storage space of the server and usage statistics help a site admin learn which workflows are most used and who are the biggest users.


Development of the Tripal Galaxy module and accompanying starter workflows was funded by the `National Science Founation award #1443040 <https://nsf.gov/awardsearch/showAward?AWD_ID=1443040>`_ and is part of the `Tripal Gateway Project <http://tripal.info/projects/tripal-gateway>`_.

.. toctree::
   :maxdepth: 1
   :caption: Galaxy Integration Overview

   ./galaxy_integration/install.rst
   ./galaxy_integration/user_quotas.rst
   ./galaxy_integration/site_wide_files.rst
   ./galaxy_integration/remote_servers.rst
   ./galaxy_integration/workflows.rst
   ./galaxy_integration/job_queue.rst
   ./galaxy_integration/viewing_usage_data.rst
   ./galaxy_integration/workflows_and_collections.rst
