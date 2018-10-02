Adding BLAST Results
====================
.. note::

  Remember you must set the ``$DRUPAL_HOME`` environment variable if you want to cut-and-paste the commands below. See :doc:`../../install_tripal/drupal_home`
  

Adding BLAST Databases
----------------------

Before we load our BLAST results we want to add some external databases.  For this tutorial we have protein BLAST results against NCBI nr and ExPASy SwissProt.  We would like the BLAST hits to be clickable such that they link back to their respective databases. To do this, we must add some additional databases.  Navigate to Tripal → Chado Modules → Databases and click the link titled Add a Database. The resulting page provides fields for adding a new database.  Add two new datbases, one for NCBI nr and the other for ExPASy SwissProt.

Use these values for adding the NCBI nr database:

.. csv-table::
  :header: "Field Name", "Value"
  
  "Name", "NCBI nr"
  "Description", "NCBI's non-redundant protein database"
  "URL", "http://www.ncbi.nlm.nih.gov/"
  "URL Prefix", "http://www.ncbi.nlm.nih.gov/protein/"

Use these values for adding the SwssProt database:

.. csv-table::
  :header: "Field Name", "Value"
  
  "Name", "ExPASy Swiss-Prot"
  "Description", "A curated protein sequence database which strives to provide a high level of annotation, a minimal level of redundancy and high level of integration with other databases"
  "URL", "http://expasy.org/sprot/"
  "URL prefix", "http://www.uniprot.org/uniprot/"


Configure Parsing of BLAST Results
----------------------------------
First, we need to ensure that the BLAST module can properly parse the BLAST hits. To do this, navigate to Tripal → Extension Modules → Tripal Blast Analyses. On this page are configuration settings for the Tripal BLAST Analysis extension module.

Within the section titled BLAST Parsing, you can specify a different, more meaningful name for the sequence library file (a.k.a. database) used for BLASTing. This name will be displayed with BLAST results. You can also provide regular expressions for parsing BLAST hits. For example, the following is a line for a match from SwissProt:

  ::

    sp|P43288|KSG1_ARATH Shaggy-related protein kinase alpha OS=Arabidopsis thaliana GN=ASK1 PE=2 SV=3


Here the hit name is `KSG1_ARATH`, the accession is `P43288`, the hit description is `Shaggy-related protein kinase alpha OS=Arabidopsis thaliana` and the organism is `Arabidopsis thaliana`. We need regular expressions to tell Tripal how to extract these unique parts from the match text. Because Tripal is a PHP application, the syntax for regular expressions follows the PHP method. Documentation for regular expressions used in PHP can be found here. The following regular expressions can be used to extract the hit name, the accession, hit description and organism for the example SwissProt line above:

.. csv-table::
  :header: "Element", "Regular Expression"
  
  "Hit Name", `^sp\|.*?\|(.*?)\s.*?$`
  "Hit Description", `^sp\|.*?\|.*?\s(.*)$`
  "Hit Accession", `^sp\|(.*?)\|.*?\s.*?$`
  "Hit Organism", `^.*?OS=(.*?)\s\w\w=.*$`

In this tutorial, we will be adding BLAST results for the two databases we just created: ExPASy SwissProt and NCBI nr. First, select ExPASy SwissProt from the drop-down menu. A form will appear:

Tripal2.0 Blast settings.png

In the form fields, add the following values:

.. csv-table::
  :header: "Field", "Value"
  
  "Title for the BLAST analysis", "(leave blank)"
  "Regular expression for Hit Name", "`^sp\|.*?\|(.*?)\s.*?$`"
  "Regular expression for Hit Description", "`^sp\|.*?\|.*?\s(.*)$`"
  "Regular expression for Hit Accession:", "`^sp\|(.*?)\|.*?\s.*?$`"
  "Regular expression for Organism", "`^.*?OS=(.*?)\s\w\w=.*$`"
  "Organism Name", "(leave blank)"

Click Save Settings.

The match accession will be used for building web links to the external database. The accession will be appended to the URL Prefix set earlier when the database record was first created.

Now select the NCBI nr database from the drop-down. NCBI databases use a format that is compatible with BLAST. Therefore, the hit name, accession and description are handled differently in the BLAST XML results. To correctly parse results from an NCBI database click the Use Genbank style parser checkbox. This should disable all other fields and is all we need for this database.  Clikc the Save Settings button.

Load the BLAST Results
----------------------
Now we can create out analysis page. Navigate to Create Content page and select the Analysis: BLAST content type. Add the following values for this analysis. In the fields set the following values:

.. csv-table::
  :header: "Field", "Value"
  
    "Analysis Name", "blastx Citrus sinensis v1.0 genes vs ExPASy SwissProt"
    "Program", "blastall"
    "Program Version", "2.2.25"
    "Algorithm", "blastx"
    "Source name", "C. sinensis mRNA vs ExPASy SwissProt"
    "Time Executed", "(today's date)"
    "Materials & Methods", "C. sinensis mRNA sequences were BLAST'ed against the ExPASy SwissProt protein database using a local installation of BLAST on in-house linux server. Expectation value was set at 1e-6"
    "BLAST Settings", "
       - Database: ExPASy SwissProt
       - BLAST XML File/Directory: /var/www/html/sites/default/files/Blastx_citrus_sinensis-orange1.1g015632m.g.fasta.0_vs_uniprot_sprot.fasta.out
       - Query Type: mRNA
       - Parameters: -p blastx -e 1e-6 -m 7
       - Submit a job to parse the XML output: checked
       - Keywords for custom search: checked"

Click the Save button. You can now see our new Analysis.

Tripal2.0 Blast analysis.png

Now we need to manually run the job to parse the BLAST results:

::

  drush trp-run-jobs --username=administrator --root=$DRUPAL_HOME

The results should now be loaded. if we visit our feature page, for feature `orange1.1g015615m` (http://localhost/feature/citrus/sinensis/mRNA/PAC%3A18136219) we should now see BLAST results by clicking the 'Homology' link in the left table of contents.

Tripal2.0 feature homology.png

Now we want to add the results for NCBI nr. Repeat the steps above to add a new analysis with the following details:

.. csv-table::
  :header: "Field", "Value"
  
  "Analysis Name", "blastx Citrus sinensis v1.0 genes vs NCBI nr"
  "Program", "blastall"
  "Program Version", "2.2.25"
  "Algorithm", "blastx"
  "Source name", "C. sinensis mRNA vs NCBI nr"
  "Time Executed", "(today's date)"
  "Materials & Methods: C. sinensis mRNA sequences were BLAST'ed against the NCBI nr protein database using a local installation of BLAST on in-house linux server. Expectation value was set at 1e-6"
  "Blast Settings", "
        - Database: NCBI nr
        - Blast XML File/Directory: /var/www/html/sites/default/files/Blastx_citrus_sinensis-orange1.1g015632m.g.fasta.0_vs_nr.out
        - Query Type: mRNA
        - Parameters: -p blastx -e 1e-6 -m 7
        - Submit a job to parse the XML output: checked
        - Keywords for custom search: checked"

Click the Save button and manually run the job:

::

  drush trp-run-jobs --username=administrator --root=$DRUPAL_HOME

Return to the example feature page to view the newly added results: http://localhost/feature/citrus/sinensis/mRNA/PAC%3A18136219
