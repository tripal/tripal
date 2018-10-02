Adding Functional Annotations
=============================
.. note::

  Remember you must set the ``$DRUPAL_HOME`` environment variable if you want to cut-and-paste the commands below. See :doc:`../install_tripal/drupal_home`
  
  
For this example we will be load functional data for our gene. To do this we will use the Blast, KEGG, and InterPro extension modules. However, these extension modules are not part of the "core" Tripal package but are available as separate extensions.  Anyone may create extensions for Tripal.  These extensions are useful for genomic data and therefore are included in this tutorial. 

To download these modules:

  ::
  
    cd $DRUPAL_HOME    
    drush pm-download tripal_analysis_blast
    drush pm-download tripal_analysis_kegg
    drush pm-download tripal_analysis_interpro

Now, enable these extension modules:

  ::
  
    drush pm-enable tripal_analysis_blast
    drush pm-enable tripal_analysis_interpro
    drush pm-enable tripal_analysis_kegg

For this example, we will use the following files which are available for downloading:

- `Citrus sinensis-orange1.1g015632m.g.iprscan.xml <http://www.gmod.org/mediawiki/images/0/0c/Citrus_sinensis-orange1.1g015632m.g.iprscan.xml>`_
- `Citrus sinensis-orange1.1g015632m.g.KEGG.heir.tar.gz <http://www.gmod.org/mediawiki/images/1/13/Citrus_sinensis-orange1.1g015632m.g.KEGG.heir.tar.gz>`_
- `Blastx citrus sinensis-orange1.1g015632m.g.fasta.0 vs uniprot sprot.fasta.out <http://www.gmod.org/mediawiki/images/e/e8/Blastx_citrus_sinensis-orange1.1g015632m.g.fasta.0_vs_uniprot_sprot.fasta.out>`_
- `Blastx citrus sinensis-orange1.1g015632m.g.fasta.0 vs nr.out <http://www.gmod.org/mediawiki/images/2/24/Blastx_citrus_sinensis-orange1.1g015632m.g.fasta.0_vs_nr.out>`_

Download these files to the /var/www/html/sites/default/files directory. To do so quickly run these commands:

  ::
  
    cd $DRUPAL_HOME 

    wget http://www.gmod.org/mediawiki/images/0/0c/Citrus_sinensis-orange1.1g015632m.g.iprscan.xml
    wget http://www.gmod.org/mediawiki/images/1/13/Citrus_sinensis-orange1.1g015632m.g.KEGG.heir.tar.gz
    wget http://www.gmod.org/mediawiki/images/e/e8/Blastx_citrus_sinensis-orange1.1g015632m.g.fasta.0_vs_uniprot_sprot.fasta.out
    wget http://www.gmod.org/mediawiki/images/2/24/Blastx_citrus_sinensis-orange1.1g015632m.g.fasta.0_vs_nr.out


.. toctree::
   :maxdepth: 1
   :caption: Table of Contents
   :glob:

   func_annots/blast
   func_annots/interpro
   func_annots/kegg
   func_annots/go
