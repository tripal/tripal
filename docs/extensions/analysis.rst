Analysis/Annotation
===================

The following modules provide support for loading annotation or analysis' into Chado and displaying them on Tripal pages.

Tripal Analysis Expression
--------------------------

A module for loading, annotating, and visualizing NCBI Biomaterials and expression data.

`Documentation <https://github.com/tripal/tripal_analysis_expression/blob/master/README.md>`__
`Repository <https://github.com/tripal/tripal_analysis_expression>`__

Tripal Analysis Blast
---------------------

This module extends the Tripal Analysis Module and provides a method for loading XML results from the NCBI blast program. Blast results appear on each feature page.

`Documentation <https://tripal.readthedocs.io/en/latest/user_guide/example_genomics/func_annots/blast.html>`__
`Repository <https://github.com/tripal/tripal_analysis_blast>`__

Tripal Analysis KEGG
--------------------

This module provides a method for loading of KEGG ortholog assignments derived from the KEGG Automated Annotation Server (KAAS). KEGG assignments appear on each feature page, and a full KEGG report is available for browsing results once uploaded.

`Repository <https://github.com/tripal/tripal_analysis_kegg>`__

Tripal Analysis Interpro
------------------------

This module provides a method for loading XML results from the InterProScan program. The module can load InterProScan XML v4 or InterProScan XML v5 generated from the command-line or web-based versions of InterProScan. Additionally, GO terms mapped by InterProScan can optionally be assigned to features.

`Documentation <https://tripal.readthedocs.io/en/latest/user_guide/example_genomics/func_annots/interpro.html>`__
`Repository <https://github.com/tripal/tripal_analysis_interpro>`__

Tripal CV-Xray
--------------

Tripal CV-Xray maps content annotations onto controlled vocabulary trees.  The end result is a browseable CV field that lets users explore ontologies and find associated content.

`Documentation <https://github.com/statonlab/tripal_cv_xray/blob/master/README.md>`__
`Repository <https://github.com/statonlab/tripal_cv_xray>`__

Tripal OrthoQuery
-----------------

The OrthoQuery module identifies orthologous genes via a Tripal database, standardizes the data for comparative analysis, performs the OrthoFinder analysis through the Tripal Galaxy API, sends the data to the user’s database profile, and provides interactive visualizations. Visualization features focus on facilitating the interrogation of large gene families, examining relationships among families, and allowing direct query of the stored orthogroups and their function. Orthoquery is currently in development.

`Repository <https://gitlab.com/TreeGenes/orthoquery>`__
