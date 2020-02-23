Importing Publications
======================
.. note::

  Remember you must set the ``$DRUPAL_HOME`` environment variable if you want to cut-and-paste the commands below. See :doc:`../install_tripal/drupal_home`
  
Tripal provides an interface for automatically and manually adding publications.

Manually Adding a Publication
-----------------------------
First, we will manually add a publication. Click the Add Tripal Content link in the administrative menu and then Publication.

.. image:: pub_import.1.png

We will add information about the Tripal publication. Enter the following values:

.. csv-table::
  :header: "Field Name", "Value"

  "Title", "Tripal v1.1: a standards-based toolkit for construction of online genetic and genomic databases."
  "Series Name", "Database"
  "Publication Year", "2013"
  "Unique Local Identifier", "Tripal v1.1: a standards-based toolkit for construction of online genetic and genomic databases."
  "Type	Journal", "Article"
  "Publication Date", "2013 Oct 25"
  "Cross Reference", "Database: PMID"
  "Accession", "24163125"
  "Authors", "Sanderson LA, Ficklin SP, Cheng CH, Jung S, Feltus FA, Bett KE, Main D"
  "Citation", "Sanderson LA, Ficklin SP, Cheng CH, Jung S, Feltus FA, Bett KE, Main D. Tripal: a construction Toolkit for Online Genome Databases. Database, Oct 25 2013. bat075"
  "Abstract", "Tripal is an open-source freely available toolkit for construction of online genomic and genetic databases. It aims to facilitate development of community-driven biological websites by integrating the GMOD Chado database schema with Drupal, a popular website creation and content management software. Tripal provides a suite of tools for interaction with a Chado database and display of content therein. The tools are designed to be generic to support the various ways in which data may be stored in Chado. Previous releases of Tripal have supported organisms, genomic libraries, biological stocks, stock collections and genomic features, their alignments and annotations. Also, Tripal and its extension modules provided loaders for commonly used file formats such as FASTA, GFF, OBO, GAF, BLAST XML, KEGG heir files and InterProScan XML. Default generic templates were provided for common views of biological data, which could be customized using an open Application Programming Interface to change the way data are displayed. Here, we report additional tools and functionality that are part of release v1.1 of Tripal. These include (i) a new bulk loader that allows a site curator to import data stored in a custom tab delimited format; (ii) full support of every Chado table for Drupal Views (a powerful tool allowing site developers to construct novel displays and search pages); (iii) new modules including ‘Feature Map’, ‘Genetic’, ‘Publication’, ‘Project’, ‘Contact’ and the ‘Natural Diversity’ modules. Tutorials, mailing lists, download and set-up instructions, extension modules and other documentation can be found at the Tripal website located at http://tripal.info."

To complete the page click the **Save** button at the bottom

Import of Publications
----------------------

Tripal supports importing of publications from remote databases such as NCBI PubMed.

Creation of an importer is an administrative function. A publication importer is created by the site administrator and consists of a set of search criteria for finding multiple publications at one time. When the importer is run, it will query the remote database, retrieve the publications that match the criteria and add them to the database. Because we loaded genomic data for Citrus sinensis we will create an importer that will find all publications related to this species.

First, navigate to **Tripal → Data Loaders → Chado Bulk Publication Importers** and click the link New Importer. You will see the following page:

.. image:: pub_import.2.png

Enter the following values in the fields:

.. csv-table::
  :header: "Field Name", "Value"

  "Remote Database", "PubMed"
  "Loader Name", "Pubs for Citrus sinensis"
  "Criteria #1", "
  - Scope: Abstract/Title
  - Search Terms: Citrus sinensis
  - is Phrase?: checked"

Now, click the 'Test Importer' button. This will connect to PubMed and search for all publications that match our provided criteria.  it may take a few minutes to complete. On the date this portion of the tutorial was written, over 800 publications were found:

.. image:: pub_import.3.png

Now, save this importer. You should see that we have one importer in the list:

.. image:: pub_import.4.png

We can use this importer to load all  publications related to Citrus sinensis from PubMed into our database (how to load these will be shown later). However, what if new publications are added? We would like this importer to be run monthly so that we can automatically add new publications as they become available. But we do not need to try to reload these 760 every time the loader runs each month. We will create a new importer that only finds publications within the last 30 days. To do this, click the link New Importer. Now, add the following criteria:

.. csv-table::
  :header: "Field Name", "Value"

  "Remote Database", "PubMed"
  "Loader Name", "Pubs for Citrus sinensis last 30 days"
  "Days since record modified", "30"
  "Criteria #1", "
  - Scope: Abstract/Title
  - Search Terms: Citrus sinensis
  - is Phrase?: checked"

Now, when we test the importer we find only 1 publications that has been added (created) to PubMed in the last 30 days:

.. image:: pub_import.5.png

Save this importer.

Next, there are two ways to import these publications. The first it to manually import them. There is a Drush command that is used for importing publications. Return to the terminal and run the following command:

::

  cd $DRUPAL_HOME
  drush trp-import-pubs --username=administrator

You should see output to the terminal that begins like this:

::

  NOTE: Loading of publications is performed using a database transaction.
  If the load fails or is terminated prematurely then the entire set of
  insertions/updates is rolled back and will not be found in the database

  Importing: Pubs for Citrus sinensis

The importer will import 100 publications at a time and pause between each set of 100 as it requests more.

Some things to know about the publication importer:

1. The importer keeps track of publications from the remote database using the publication accession (e.g. PubMed ID).
2. If a publication with an accession (e.g. PubMed ID) already exists in the local database, the record will be updated.
3. If a publication in the local database matches by title, journal and year with one that is to be imported, then the record will be updated. Y
4. Run the newly created Tripal Job to finish:

  ::

    cd $DRUPAL_HOME
    drush trp-run-jobs --user=administrator

The second way to import publications is to add an entry to the UNIX cron. We did this previously for the Tripal Jobs management system when we first installed Tripal. We will add another entry for importing publications. But first, now that we have imported all of the relevant pubs, we need to return to the importers list at **Tripal → Data Loaders → Chado Publication Importers** and disable the first importer we created. We do not want to run that importer again, as we've already imported all historical publications on record at PubMed. Click the edit button next to the importer named Pubs for Citrus sinensis, click the disable checkbox and then save the template. The template should now be disabled.

Now we have the importer titled **Pubs for Citrus sinensis last 30 days** enabled. This is the importer we want to run on a monthly basis. The cron entry will do this for us. On the terminal open the crontab with the following command:

::

  sudo crontab -e

Now add the following line to the bottom of the crontab:

::

  30 8 1,15 * *  su - www-data -c '/usr/local/drush/drush -r [DRUPAL_HOME] -l http://[site url] trp-import-pubs --report=[your email] > /dev/null'

Where

- [site url] is the full URL of your site
- [your email] is the email address of the user that should receive an email containing a list of publications that were imported. You can separate multiple email addresses with a comma.
- [DRUPAL_HOME] is the directory where Drupal is installed

The cron entry above will launch the importer at 8:30am on the first and fifteenth days of the month. We will run this importer twice a month in the event it fails to run (e.g. server is down) at least one time during the month.
