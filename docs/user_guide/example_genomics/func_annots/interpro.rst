Adding InterProScan Results
===========================

.. note::

  Remember you must set the ``$DRUPAL_HOME`` environment variable if you want to cut-and-paste the commands below. See :doc:`../../install_tripal/drupal_home`

For this tutorial, these results were obtained by using a local installation of InterProScan installed on a computational cluster. However, you may choose to use Blast2GO or the online InterProScan utility. Results should be saved in ``XML`` format.


What is InterProScan?
---------------------
To learn more about InterProScan, please visit https://www.ebi.ac.uk/interpro/


Create the Analysis Page
-------------------------

  .. note::

    It is always recommended to create an analysis page anytime you import data. The purpose of the analysis page is to describe how the data being added was derived or collected.


    Analysis Name: InterPro Annotations of C. sinensis v1.0
Program: InterProScan
Program Version: 4.8
Algorithm: iprscan
Source name: C. sinensis v1.0 mRNA
Time Executed: (today's date)
Materials & Methods: C. sinensis mRNA sequences were mapped to IPR domains and GO terms using a local installation of InterProScan executed on a computational cluster. InterProScan date files used were MATCH_DATA_v32, DATA_v32.0 and PTHR_DATA v31.0.
InterPro Settings
InterProScan XML File/Directory: /var/www/html/sites/default/files/Citrus_sinensis-orange1.1g015632m.g.iprscan.xml



Check the box 'Submit a job to parse the Interpro XML output'
Check the box 'Load GO terms'
Parameters: iprscan -cli -goterms -ipr -format xml
Query Type: mRNA




Import the InterProScan XML results
------------------------------------


Next, we will load InterProScan results for our citrus gene.  To do this, navigate to **Tripal > Data Loaders > Chado InterProScan XML results loader**.  The following page will be presented:

.. image:: blast4.png

The top section of this page provides multiple methods for providing results file: via an upload interface, specifying a remote URL or a file path that is local to the server.  Most likely, you will always upload or provide a remote URL.  However, we download the file earlier, and stored them here: ```$DRUPAL_HOME/sites/default/files```.  So, in this case we can use the path on the local server.  Provide the following value for this form:

.. csv-table::
  :header: "Field", "Value"

  "Server path", "sites/default/files/Blastx_citrus_sinensis-orange1.1g015632m.g.fasta.0_vs_uniprot_sprot.fasta.out"
  "Analysis", "blastx Citrus sinensis v1.0 genes vs ExPASy SwissProt (blastall 2.2.25, Citrus sinensis mRNA vs ExPASy SwissProt)"
  "Database", "ExPASy SwissProt"
  "BLAST XML File Extension", "out"
  "Query Type", "mRNA"

.. note::

  For the **Server path** we need not give the full path.  Because we downloaded the files into the Drupal directory we can leave off any preceeding path and Tripal will resolve the path.  Otherwise we could provide the full path.

.. note::

  Specifying **ExPASy SwissProt** as the database will allow the importer to use the database configuration settings we entered earlier.

Clicking the **Import BLAST file** will add a job which we can manually execute with the following command:
