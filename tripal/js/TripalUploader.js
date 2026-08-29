/**
 * See the Tripal Uploader API documentation for instructions to use this class.
 */

(function (jQuery, Drupal) {

  "use strict";

  class TripalUploader {

    /**
     * Class constructor function.
     */
    constructor() {
      // Holds the list of files organized by category and index.
      this.files = {};
 
      // Holds upload table definitions.
      // The tables array will have the following keys:
      //   - tname: the name of the HTML table containing the file.
      //   - category: the category within the table to which the file belongs.
      //   - index: the index of the file in the table.
      //   - url: The URL at the remote server where the file will uploaded.
      this.tables = {};
    }

    /**
     * Adds a file to the TripalUploader object.
     *
     * @param file
     *   The HTML5 file object.
     * @param options
     *   A set of key value pairs of the following
     *     - tname: the name of the HTML table containing the file.
     *     - category: the category within the table to which the file belongs.
     *     - index: the index of the file in the table.
     *     - url: The URL at the remote server where the file will uploaded.
     */
    addFile(file, options) {
      const tname = options['tname'];
      const category = options['category'];
      const i = options['i'];
      const url = options['url'];

      // Make sure the file type is allowed. If there are no file types,
      // then anything is allowed.
      if (this.tables[tname]['allowed_types'] && this.tables[tname]['allowed_types'].length > 0) {
        const allowed_types = this.tables[tname]['allowed_types'];
        const matches = file.name.match(/^.*\.(.+)$/);
        if (!matches) {
          alert('Please provide a file with a valid extension.');
          return null;
        }
        const type = matches[1];
        let found = false;
        for (let j = 0; j < allowed_types.length; j++) {
          if (allowed_types[j] === type) {
            found = true;
          }
        }
        if (!found) {
          alert('Please provide a file with a valid extension. The following are allowed: ' + allowed_types.join(','));
          return null;
        }
      }

      if (!(category in this.files)) {
        this.files[category] = {}
      }
      const guf_options = {
        'parent' : this,
        'index' : i,
        'url' : url,
        'category' : category,
        'tname' : tname,
        'progress' : category + '-progress-' + i,
        'links' : category + '-links-' + i,
        'module' : this.tables[tname]['module']
      }

      const guf = new Drupal.TripalUploadFile(file, guf_options);
      this.files[category][i] = guf;
      return guf;
    }

    /**
     * Removes a file from the TripalUploader object.
     */
    removeFile(tname, category, i) {
      if (category in this.files) {
        if (i in this.files[category]) {
          delete this.files[category][i];
        }
      }
      this.setTarget(tname);
    }

    /**
     * Returns the maximum index of all uploaded files.
     */
    getMaxIndex(category) {
      let index = 0;
      if (category in this.files) {
        for (let i in this.files[category]) {
          if (i > index) {
            index = i;
          }
        }
      }
      return index;
    }

    /**
     * Return the number of uploaded files.
     */
    getNumFiles(category) {
      let count = 0;
      if (category in this.files) {
        for (let i in this.files[category]) {
          count = count + 1;
        }
      }
      return count;
    }

    /**
     * Returns files in a specified category.
     */
    getCategoryFiles(category) {
      if (!(category in this.files)) {
        return [];
      }
      return this.files[category];
    }

    /**
     * Retrieve a file given its category and index within that category.
     */
    getCategoryFile(category, i) {
      if (category in this.files && i in this.files[category]) {
        return this.files[category][i];
      }
      return null;
    }

    /**
     * Cancel a file upload using its category and index within that category.
     */
    cancelFile(category, i) {
      if (category in this.files) {
        this.files[category][i].cancel();
      }
    }

    /**
     * Start upload of all files within a category.
     */
    start(category) {
      if (category in this.files) {
        for (let i in this.files[category]) {
          this.files[category][i].start();
        }
      }
    }

    /**
     * Update status of files in one or more categories.
     */
    updateProgress(categories) {
      if (!Array.isArray(categories)) {
        categories = [categories];
      }

      for (let i in categories) {
        if (categories[i] in this.files) {
          for (let j in this.files[categories[i]]) {
            this.files[categories[i]][j].updateStatus();
          }
        }
      }
    }

    /**
     * Cancel all uploads within a category.
     */
    reset(category) {
      if (category in this.files) {
        for (let i in this.files[category]) {
           this.files[category][i].cancel();
        }
        this.files[category] = [];
      }
    }

    /**
     * Generates HTML for the file upload button.
     */
    getFileButton(tname, category, i) {
      const button_name = tname + '--' + category + '-upload-' + i;
      const element = '<input id="' + button_name + '" class="tripal-chunked-file-upload" type="file" ready="false">';

      return {
        'name' : button_name,
        'element' : element
      }
    }

    /**
     * Return file information given a button identifier.
     */
    parseButtonID(id) {
      // Get the category and index for this file.
      const tname = id.replace(/^(.+)--(.+)-upload-(.+)$/, '$1');
      const category = id.replace(/^(.+)--(.+)-upload-(.+)$/, '$2');
      const index = id.replace(/^(.+)--(.+)-upload-(.+)$/, '$3');

      return {
       'tname' : tname,
       'category' : category,
       'index' : index
      }
    }

    /**
     * Initializes the loader for a given HTML table.
     *
     * The TripalUploader supports two types of tables, a table for
     * uploading paired data (e.g. RNA-seq) and single files. This function
     * replaces the body of an existing table as new files and updates
     * the table as files are uploaded.
     *
     * @param tname
     *   The name of the table. For single files it is best to name the
     *   table the same as the file category. For paired data it is best
     *   to use a name that represents both categories.
     * @param options
     *   An associative array that contains the following keys:
     *   table_id: The HTML id of the table. For single data, the table
     *     must already have 4 columns with headers (file name,
     *     size, progress and action). For paired data, the table
     *     must already have 8 columns, which are the same as the
     *     single table but with two sets.
     *   category: An array. It must contain the list of categories that
     *     this table manages. For paired data include two categories.
     *     This is the category of the file when saved in Tripal.
     *   submit_id: The HTML id of the submit button.
     *   module: The name of the module managing the table.
     *   cardinality: (optional) The number of files allowed. Set to 0 for
     *     unlimited. Default is 0.
     *   target_id: (optional). The HTML id of the hidden field in the form
     *     where the file ID will be written to this field. This only
     *     works if cardinality is set to 1.
     *   allowed_types: (optional). An array of allowed file extensions (e.g.
     *     fasta, fastq, fna, gff3, etc.).
     */
    addUploadTable(tname, options) {
      const categories = options['category'];
      const submit_id = options['submit_id'];

      // Save the table ID for this category.
      if (!(tname in this.tables)) {
        this.tables[tname] = {};
      }
      this.tables[tname]['table_id'] = options['table_id'];
      this.tables[tname]['category'] = categories;
      this.tables[tname]['submit_id'] = submit_id;
      this.tables[tname]['target_id'] = options['target_id'];
      this.tables[tname]['cardinality'] = options['cardinality'];
      this.tables[tname]['module'] = options['module'];
      this.tables[tname]['allowed_types'] = options['allowed_types'];
      this.updateTable(categories[0]);
      this.enableSubmit(submit_id);
    }

    /**
     * Adds a click event to the submit button that starts the upload.
     */
    enableSubmit(submit_id) {
      let categories = [];

      // Iterate through all of the tables that use this submit button
      // and collect all the categories. We want to update them all.
      for (let tname in this.tables) {
        if (this.tables[tname]['submit_id'] === submit_id){
          for (let i = 0; i < this.tables[tname]['category'].length; i++) {
            categories.push(this.tables[tname]['category'][i])
          }
        }
      }
      jQuery(submit_id)
        .off('click.tripalUploader')
        .on('click', () => {
        for(let i = 0; i < categories.length; i++) {
          this.start(categories[i]);
        }
      });
    }

    /**
     * Updates the table for the given file category.
     */
    updateTable(category) {
      // Iterate through all of the tables that are managed by this object.
      for (let tname in this.tables) {
        // Iterate through all of the categories on each table.
        for (let i = 0; i < this.tables[tname]['category'].length; i++) {
          // If the category of the table matches then update it.
          if (this.tables[tname]['category'][i] === category) {
            this.updateTableHTML(tname, this.tables[tname]['category']);
            this.updateProgress(this.tables[tname]['category']);
            return;
          }
        }
      }
    }

    /**
     * Sets the table's target field with the file id.
     *
     * @param $file_id
     *   The Tripal file_id
     * @param $tname
     *   The name of the HTML table where the file is kept.
     * @param $category
     *   The name of the category to which the file belongs.
     */
    setTarget(tname) {
      const categories = this.tables[tname]['category'];
      const num_categories = categories.length;
      const target_id = this.tables[tname]['target_id'];

      if (target_id) {
        let fids = [];

        // Iterate through the file categories.
        for (let c = 0; c < num_categories; c++) {
          const files = this.getCategoryFiles(categories[c]);
          let cat_fids = [];

          jQuery.each(files, function (idx, file) {
            cat_fids.push(file.file_id);
          });
          fids.push(cat_fids.join('|'));
        }
        jQuery('#' + target_id).val(fids.join(','));
      }
    }

    /**
     * Update the HTML table listing uploaded files.
     *
     * @param tname
     *   The HTML table name.
     * @param categories
     *   File categories to process.
     */
    updateTableHTML(tname, categories) {
      if (!Array.isArray(categories)) {
        categories = [categories];
      }

      const max_rows_allowed = this.tables[tname]['cardinality'];
      const table_id = this.tables[tname]['table_id'];
      let content = '';
      let buttons = [];

      // Note that the variable indexes is never reassigned, possible bug.
      let indexes = {};
      let row_has_file;
      let row;
      let row_buttons;
      let highest_index = 0;

      for (let cat_idx in categories) {
        for (let file_idx in this.getCategoryFiles(categories[cat_idx])) {
          indexes[file_idx] = file_idx;
          highest_index = ((file_idx > highest_index) ? file_idx : highest_index);
        }
      }
      const rows_with_files = Object.keys(indexes).length;

      for (let idx in indexes) {
        [row_has_file, row, row_buttons] = this.getRowHTML(idx, tname, categories)
        if (row_has_file) {
          content += row;
          buttons = buttons.concat(row_buttons);
        }
      }

      if (!max_rows_allowed || max_rows_allowed === 0 || max_rows_allowed > rows_with_files) {
        [row_has_file, row, row_buttons] = this.getRowHTML(highest_index + 1, tname, categories)
        content += row;
        buttons = buttons.concat(row_buttons);
      }

      jQuery(table_id + ' > tbody').html(content);
      for (let i in buttons) {
        this.enableFileButton(buttons[i]['name']);
      }
    }

    /**
     * Return HTML for a single row for the table of uploaded files.
     */
    getRowHTML(rownum, tname, categories) {
      let row_buttons = [];
      let row = '<tr class="' + ((rownum % 2) ? 'even' : 'odd') + '">';
      let row_has_file = false;

      if (!Array.isArray(categories)) {
        categories = [categories];
      }

      for (let cat of categories) {
        const file = this.getCategoryFile(cat, rownum);
        if (file) {
          row += '<td>' + file.getFileName() + '</td>';
          row += '<td>' + file.getFileSize(true) + '</td>';
          row += '<td>' + file.getProgressBar() + '</td>';
          row += '<td>' + file.getLinks() + '</td>';
          row_has_file = true;
        }
        else {
          const button = this.getFileButton(tname, cat, rownum);
          row_buttons.push(button);
          row += '<td colspan="4">' + button['element'] + '</td>';
        }
      }
      row += '</tr>';

      return [row_has_file, row, row_buttons];
    }

    /**
     * Adds a function to the change event for the file button.
     *
     * This causes a new file to be added to this object when it is clicked.
     * The button is added by updateUploadTable.
     */
    enableFileButton(button_name) {

      // If the button already exists then it's already setup so just
      // return.
      if(jQuery('#' + button_name).attr('ready') === 'true') {
        return;
      }

      // When the button provided by the TripalUploader class is clicked
      // then we need to add the files to the object. We must have this
      // function so that we can set the proper URL.
      jQuery('#' + button_name).on('change', (e) => {
        const id = e.currentTarget.id;

        // Get the HTML5 list of files to upload.
        const hfiles = e.target.files;

        // Let the TripalUploader object parse the button ID to give us
        // the proper category name and index.
        const button = this.parseButtonID(id);
        const tname = button['tname'];
        const category = button['category'];
        const index = button['index'];

        // Add the file(s) to the uploader object.
        for (let i = 0; i < hfiles.length; i++) {
          const f = hfiles[i];
          const baseurl = window.location.protocol + '//' + window.location.host + drupalSettings.path.baseUrl;
          const options = {
            // Files are managed by tables.
            'tname' : tname,
            // Files can be categorized to separate them from other files.
            'category': category,
            // The index is the numeric index of the file. Files are ordered
            // by their index. The file with an index of 0 is always ordered first.
            'i': index,
            // The URL at the remote server where the file will uploaded.
            'url' : baseurl + 'tripal/upload/' + category,
            };
            this.addFile(f, options);

            // We need to update the upload table and the progress. The
          // information for which table to update is in the this.tables
          // array.
          this.updateTable(category);
        }
      });
      jQuery('#' + button_name).attr('ready', 'true');
    }
  }

  // Export the objects to Drupal for use in other JS files.
  Drupal.TripalUploader = TripalUploader;

})(jQuery, Drupal);
