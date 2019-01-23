
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

The Multi-Chado module is an extension for Tripal 2.x and 3.x (dev branch under testing) that can be used to install more than one Chado instance across different schemata of your choice and it also enables the use of different PostgreSQL database credentials allowing the administrator to do fine tuning of database accesses. With it you can use a *same Drupal instance* for both a *public* and a *private* Chado instance, have *different releases* or *separated species*, provide a *sandbox*, run *tests* on a blank instance and more (dev-staging-prod, etc.).

`Documentation <http://cgit.drupalcode.org/tripal_mc/plain/README.md?h=7.x-1.x>`__
`Repository <https://www.drupal.org/project/tripal_mc>`__
