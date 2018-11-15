Visualization/Display
======================

The following modules provide specialized displays for Tripal content types.

Mainlab Tripal Data Display
----------------------------

Mainlab Tripal Data Display contains a set of Drupal/PHP templates that organize and extend the default display of the biological data hosted on a Tripal-enabled site (i.e. http://tripal.info). Supported data type includes orgainsm, marker, QTL, germplasm (stock), map (featuremap), project, heritable phenotypic marker (MTL), environment (ND geolocation), haplotype block, polymorphism, eimage, generic gene (genes created by parsing Genbank files using the Mainlab 'tripal_genbank_parser' module), feature, and pub. Each of the templates can be turned on/off as desired.

`Documentation <https://gitlab.com/mainlabwsu/mainlab_tripal/blob/master/README.md>`__
`Repository <https://gitlab.com/mainlabwsu/mainlab_tripal>`__

Tripal Fancy Fields
-------------------

This module provides additional fields for use with Tripal 3. The current version provides a single-series chart field that can be displayed as a pie, donut, or bar chart, as well as, a simple table.

`Documentation <https://github.com/tripal/trpfancy_fields/blob/master/README.md>`__
`Repository <https://github.com/tripal/trpfancy_fields>`__

TripalMap
----------

TripalMap MapViewer module displays map data stored in Chado. MapViewer provides interfaces to view all linkage groups of a map, choose a linkage group and zoom in to a specific region of a linkage group, compare maps that share the same markers and change colors of markers/QTL. The interface can be integrated into Tripal map page and hyperlinked to/from any Tripal page that are displayed in maps (marker, QTL, heritable morphological marker and/or gene). The admin page allows site developers some flexibility in the display pattern. Allows Tripal based databases to provide users functionality similar to GMOD-CMap without having to maintain a separate underlying database. The map display is built from the data in Materialized Views for both better performance and flexibility of the way the data is stored in

`Documentation <https://gitlab.com/mainlabwsu/tripal_map/blob/master/README.md>`__
`Repository <https://gitlab.com/mainlabwsu/tripal_map>`__
