
In Development
==============

The following modules are not yet ready for production or not fully Tripal 3 compatible.


TripalMap
-----------

TripalMap MapViewer module displays map data stored in Chado. MapViewer provides interfaces to view all linkage groups of a map, choose a linkage group and zoom in to a specific region of a linkage group, compare maps that share the same markers and change colors of markers/QTL. The interface can be integrated into Tripal map page and hyperlinked to/from any Tripal page that are displayed in maps (marker, QTL, heritable morphological marker and/or gene). The admin page allows site developers some flexibility in the display pattern.

`Documentation <https://gitlab.com/mainlabwsu/tripal_map/blob/master/README.md>`__
`Repository <https://gitlab.com/mainlabwsu/tripal_map>`__

Tripal Apollo
--------------

Tripal Apollo lets you manage user accounts for your JBrowse Apollo instances on your Tripal site.  Provides a form to request apollo access, an Apollo instance content type that connects to Chado Organisms, and an administrative interface for managing requests.

`Documentation <https://tripal-apollo.readthedocs.io/en/latest/>`__
`Repository <https://github.com/NAL-i5K/tripal_apollo>`__

Tripal Multi-Chado
------------------

The Multi-Chado module is an extension for Tripal 2.x and 3.x (dev branch under testing) that can be used to install more than one Chado instance across different schemata of your choice and it also enables the use of different PostgreSQL database credentials allowing the administrator to do fine tuning of database accesses. For instance, with this module, you can host on a *same Drupal instance* (config, tools and users) both a *public* and one or more *private* Chado instances. You could also have different public instances for *different releases* of a same genome sequencing or event *different species*. It can also alow you do provide a Chado *sandbox* to allow users to safely modify Chado data and see the changes on the site. It can be used to run *tests* against a blank Chado instance (see https://github.com/tripal/tripal_simpletest/). And you may even think of other kind of uses (dev-staging-prod, etc.)... The dev branch is supposed to work well with Tripal 3 but has not been extensively tested yet. A companion module (tripal_mc_selector) is provided to allow easy Chado instance switching. Several hooks are available for module developers in order to automatically switch Chado instance according to a given context.

`Documentation <http://cgit.drupalcode.org/tripal_mc/plain/README.md?h=7.x-1.x>`__
`Repository <https://www.drupal.org/project/tripal_mc>`__
