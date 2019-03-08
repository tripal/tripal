<p>The bulk loader is a tool that Tripal provides for loading of data contained
    in tab
    delimited files. Tripal supports loading of files in standard formats (e.g.
    FASTA, GFF, OBO),
    but Chado can support a variety of different biological data types and there
    are often no
    community standard file formats for loading these data. For example, there
    is no file
    format for importing genotype and phenotype data. Those data can be stored
    in the feature,
    stock and natural diversity tables of Chado. The Bulk Loader was introduced
    in Tripal v1.1
    and provides a web interface for building custom data loader. In short, the
    site developer
    creates the bulk loader "template". This template can then be used and
    re-used for any tab
    delimited file that follows the format described by the template.
    Additionally, bulk loading
    templates can be exported allowing Tripal sites to share loaders with one
    another. Loading
    templates that have been shared are available on the Tripal website here:
  <?php print l('http://tripal.info/extensions/bulk-loader-templates', 'http://tripal.info/extensions/bulk-loader-templates'); ?>
    .</p>
<br/>
<h3>General Usage</h3>
<ol>
    <li><strong>Plan how to store your data.</strong>
        <br/>This is the most important and often the most difficult step. This
        is because it requires familiarity with the Chado database schema and
        due to the
        flexibility of the schema, you may be able to store your data multiple
        ways. It is
        considered best practice to consult the GMOD website and the Chado
        community (via the
      <?php print l('gmod-schema mailing list', 'https://lists.sourceforge.net/lists/listinfo/gmod-schema'); ?>
        ) when deciding how to store data.
    </li>
    <li><strong>Create a new Bulk Loading Template to map the columns from your
            data file to chado.</strong>
        <br/>Creating a new template can be done by clicking on the "Templates"
        tab above
        and then the "Add Template" link. Note that the template is designed
        with a single line
        from your file in mind. Furthermore, the term "record" refers to a
        single entry in chado
        and the term "field" refers to a column in a specific chado table.
    </li>
    <li><strong>Create a Bulk Loader Job with file-specific details that uses
            your template to load a
            specific file.</strong>
        <br/>Create a Bulk Loading Job by clicking on the "Jobs" tab above and
        then "Add Bulk
        Loading Job". Remember to select the template you just created and to
        ensure that you
        provide the absolute path to the file. Note: The file must already be
        uploaded to the
        same server as your Drupal installation and must be readable by the
        command-line
        user who executes the tripal job.
    </li>
</ol>
<p>For the full tutorial, see
    the <?php print l('Tripal User Manual: The Bulk Loader', 'http://tripal.info/node/109'); ?>
    .</p>