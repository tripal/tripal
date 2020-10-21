Visualization/Display
======================

The following modules provide specialized displays for Tripal content types.

Analyzed Phenotypes
--------------------

.. image:: https://tripal.readthedocs.io/en/7.x-3.x/_images/Tripal-Gold.png
  :target: https://tripal.readthedocs.io/en/7.x-3.x/extensions/module_rating.html#Gold
  :alt: Tripal Rating: Gold

This module provides support and visualization for partially analyzed data stored in a modified GMOD Chado schema. It is meant to support large scale phenotypic data through backwards compatible improvements to the Chado schema including the addition of a project and stock foreign key to the existing phenotype table, optimized queries and well-chosen indexes.

`Documentation <https://analyzedphenotypes.readthedocs.io/en/latest/index.html>`__
`Repository <https://github.com/UofS-Pulse-Binfo/analyzedphenotypes>`__

CvitEmbed
----------

This module integrates `CViTjs <https://github.com/LegumeFederation/cvitjs>`__ with Tripal to provide whole-genome visualizations. It creates one page per plot and makes them accessible via the Drupal Navigation menu.

`Documentation <https://github.com/UofS-Pulse-Binfo/cvitembed/blob/master/README.md>`__
`Repository <https://github.com/UofS-Pulse-Binfo/cvitembed>`__

Mainlab Tripal Data Display
----------------------------

Mainlab Tripal Data Display contains a set of Drupal/PHP templates that organize and extend the default display of the biological data hosted on a Tripal-enabled site (i.e. http://tripal.info). Supported data type includes organism, marker, QTL, germplasm (stock), map (featuremap), project, heritable phenotypic marker (MTL), environment (ND geolocation), haplotype block, polymorphism, eimage, generic gene (genes created by parsing Genbank files using the Mainlab ```tripal_genbank_parser`` module), feature, and pub. Each of the templates can be turned on/off as desired.

`Documentation <https://gitlab.com/mainlabwsu/mainlab_tripal/blob/master/README.md>`__
`Repository <https://gitlab.com/mainlabwsu/mainlab_tripal>`__

ND Genotypes
-------------

.. image:: https://tripal.readthedocs.io/en/7.x-3.x/_images/Tripal-Gold.png
  :target: https://tripal.readthedocs.io/en/7.x-3.x/extensions/module_rating.html#Gold
  :alt: Tripal Rating: Gold

This module provides support and visualization of genotypic data stored in a modified GMOD Chado schema. The 3.x branch of this module represents a shift towards support for large scale genotypic datasets through backwards compatible improvements to the Chado schema including a new gathering table for genotypes (genotype_call) modeled after the Chado phenotype table, optimized queries and well-chosen indexes.

`Documentation <https://nd-genotypes.readthedocs.io/en/latest/>`__
`Repository <https://github.com/UofS-Pulse-Binfo/nd_genotypes>`__

Phylotree
-------------------

This extension provides a simple file formatter for `Newick <http://evolution.genetics.washington.edu/phylip/newicktree.html>`__ tree files using
the Javascript library `Phylotree <https://github.com/veg/phylotree.js/tree/master>`__ for display.

`Documentation <https://cgit.drupalcode.org/phylotree/tree/README.md>`__
`Repository <https://www.drupal.org/project/phylotree>`__

Tripal Fancy Fields
-------------------

This module provides additional fields for use with Tripal 3. The current version provides a single-series chart field that can be displayed as a pie, donut, or bar chart, as well as, a simple table.

`Documentation <https://github.com/tripal/trpfancy_fields/blob/master/README.md>`__
`Repository <https://github.com/tripal/trpfancy_fields>`__

TripalMap
-----------

TripalMap MapViewer module displays map data stored in Chado. MapViewer provides interfaces to view all linkage groups of a map, choose a linkage group and zoom in to a specific region of a linkage group, compare maps that share the same markers and change colors of markers/QTL. The interface can be integrated into Tripal map page and hyperlinked to/from any Tripal page that are displayed in maps (marker, QTL, heritable morphological marker and/or gene). The admin page allows site developers some flexibility in the display pattern.

`Documentation <https://gitlab.com/mainlabwsu/tripal_map/blob/master/README.md>`__
`Repository <https://gitlab.com/mainlabwsu/tripal_map>`__
