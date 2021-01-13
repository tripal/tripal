
How to use Tripal Console
=============================

Tripal Console is a command-line extension which uses Drupal Console. To use Tripal Console, (1) open up the command line to your site, (2) navigate into your drupal root, (3) execute commands! Which commands you may ask? Executing the following will tell you all the commands at your disposal!

.. code::

  cd [Drupal Root]/web
  ../vendor/bin/drupal --help

Then to determine the specifics for a given command, execute the command with the help option:

.. code::

  ../vendor/bin/drupal tripal:generate:fieldType --help

  Usage:
    tripal:generate:fieldType [options]
    trpgen-fieldType

  Options:
        --module=MODULE                        The machine name of the module to generate the content in.
        --default-widget=DEFAULT-WIDGET        The machine name of the default widget for this field type. (e.g. obi__organism_default_widget)
        --default-formatter=DEFAULT-FORMATTER  The machine name of the default formatter for this field type. (e.g. obi__organism_default_formatter)
        --vocab-short=VOCAB-SHORT              The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
        --vocab-name=VOCAB-NAME                The full name of the vocabulary.
        --vocab-description=VOCAB-DESCRIPTION  The description of the vocabulary.
        --term-name=TERM-NAME                  The name of the term.
        --term-accession=TERM-ACCESSION        The unique ID (i.e. accession) of the term.
        --term-definition=TERM-DEFINITION      The definition of the term.
        --chado-table=CHADO-TABLE              The table in Chado that the field maps to.
        --chado-column=CHADO-COLUMN            The column of the table in Chado where the value comes from.
        --chado-base=CHADO-BASE                The base table.
        --type-class[=TYPE-CLASS]              The class name for the field type (e.g. OBIOrganismItem)
        --type-label[=TYPE-LABEL]              The label for the field type (e.g. Organism)
        --type-plugin-id[=TYPE-PLUGIN-ID]      The machine name for the field type (e.g. obi__organism)
        --type-description[=TYPE-DESCRIPTION]  A short description of the type for administrators (e.g. the term description)

  Help:
   Generate Tripal 4 Field Type file following coding standards

Interactive Execution
-----------------------
Just execute the command with no paramters and let it ask you for input! For example, run `drupal tripal:generate:fieldType`. This can be run from any directory within your site since it asks you the module you want to create the fieldType in.

.. code::

  drupal tripal:generate:fieldType

   The machine name of the module to generate the content in.:
   > tripal_chado
   The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).:
   > SO
   The full name of the vocabulary.:
   > 0000704

In the above example, you can see how the command prompts you for each option with helpful text to guide to, often even an example! Then you just enter the value and it will keep prompting until all the parameters have been addressed.

Autonomous Execution
-----------------------

If you don't want to interact with the command (i.e. within a script), you can simply supply all the parameters on the command-line. You can use `drupal [command] --help` to find out what options are available. Then you can provide any combination of options on the command-line and if you missed any required ones it will just prompt for them.

.. code::

  drupal tripal:generate:fieldType --module=mycustommodule \
    --chado-table=organism --chado-column=genus --chado-base=organism \
    --vocab-short=TAXRANK --vocab-name="Taxonomic Rank" \
    --term-name=genus --term-accession=0000005 
