/**
 * See the Tripal Uploader API documenation for instructions to use this class.
 */

(function($) {

  "use strict";
  
  /**
   * The constructor function.
   */
  var TripalUploader = function() {

    // Holds the list of files and organizes them by category and then
    // by an index number.
    this.files = {};
    
    // The tables array will have the following keys:
    //
    // tname: the name of the HTML table containing the file.
    // category:  the category within the table to which the file belongs.
    // index:  the index of the file in the table.
    // url: The URL at the remote server where the file will uploaded.
    this.tables = {};

    /**
     * Adds a file to the TripalUploader object
     * 
     * @param file
     *   The HTML5 file object.
     * @param options
     *   A set of key value pairs of the following
     *     - tname: the name of the HTML table containing the file.
     *     - category:  the category within the table to which the file belongs.
     *     - index:  the index of the file in the table.
     *     - url: The URL at the remote server where the file will uploaded.
     */
    this.addFile = function(file, options) {
      var tname = options['tname'];
      var category = options['category'];
      var i = options['i'];
      var url = options['url'];
      var self = this;
      
      // Make sure the file type is allowed.  If there are no file types
      // then anything is allowed.
      if (this.tables[tname]['allowed_types'] && this.tables[tname]['allowed_types'].length > 0) {
        var allowed_types = this.tables[tname]['allowed_types'];
        var matches = file.name.match(/^.*\.(.+)$/);
        if (!matches) {
          alert('Please provide a file with a valid extension.');
          return null;
        }
        var type = matches[1];
        var j;
        var found = false;
        for (j = 0; j < allowed_types.length; j++) {
          if (allowed_types[j] == type) {
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
      var options = {
        'parent' : self,
        'index' : i,
        'url' : url,
        'category' : category,
        'tname' : tname,
        'progress' : category + '-progress-' + i,
        'links' : category + '-links-' + i,
        'module' : this.tables[tname]['module']
      }
      
      var guf = new TripalUploadFile(file, options)
      this.files[category][i] = guf;
      return guf
    };
    /**
     * 
     */
    this.removeFile = function(tname, category, i) {
      if (category in this.files) {
        if (i in this.files[category]) {
          delete this.files[category][i];
        }
      }
      this.setTarget(tname);
    }
    /**
     * 
     */
    this.getMaxIndex = function(category) {
      var index = 0;
      if (category in this.files) {
        for (var i in this.files[category]) {
          if (i > index) {
            index = i;
          }
        }
      }
      return index;
    }
    /**
     * 
     */
    this.getNumFiles = function(category) {
      var count = 0;
      if (category in this.files) {
        for (var i in this.files[category]) {
          count = count + 1;
        }
      }
      return count;
    }
    /**
     * 
     */
    this.getCategoryFiles = function(category) {
      if (!(category in this.files)) {
        return [];
      }
      return this.files[category];
    };
    /**
     *
     */
    this.getCategoryFile = function(category, i) {
      if (category in this.files && i in this.files[category]) {
        return this.files[category][i];
      }
      return null;
    }
    /**
     * 
     */
    this.cancelFile = function(category, i) {
      if (category in this.files) {
        this.files[category][i].cancel();
      }
    };
    /**
     * 
     */
    this.start = function(category) {
      if (category in this.files) {
        for (var i in this.files[category]) {
          this.files[category][i].start();
        }
      }
    };
    /**
     * 
     */
    this.updateProgress = function(categories) {
      if (typeof(categories) != "object") {
        categories = [categories];
      }

      for (var i in categories) {
        if (categories[i] in this.files) {
          for (var j in this.files[categories[i]]) {
            this.files[categories[i]][j].updateStatus();
          }
        }
      }
    };
    /**
     * 
     */
    this.reset = function(category) {
      if (category in this.files) {
        for (i in this.files[category]) {
           this.files[category][i].cancel();
        }
        this.files[category] = [];
      }
    }
    
    /**
     * 
     */
    this.getFileButton = function(tname, category, i) {
      var button_name = tname + '--' + category + '-upload-' + i;
      var element = '<input id="' + button_name + '" class="tripal-chunked-file-upload" type="file" ready="false">';
      
      return {
        'name' : button_name,
        'element' : element,
      }
    }
    
    /**
     * 
     */
    this.parseButtonID = function(id) {
      // Get the category and index for this file.
      var tname = id.replace(/^(.+)--(.+)-upload-(.+)$/, '$1');
      var category = id.replace(/^(.+)--(.+)-upload-(.+)$/, '$2');
      var index = id.replace(/^(.+)--(.+)-upload-(.+)$/, '$3');
      
      return {
       'tname' : tname,
       'category' :  category, 
       'index' : index
      };
    }
    
    /**
     * Initializes the loader for a given HTML table.
     * 
     * The TripalUploader supports two types of tables, a table for
     * uploading paired data (e.g. RNA-seq) and single files.  This function
     * replaces the body of an existing table as new files and updates
     * the table as files are uploaded.
     * 
     * @param tname
     *   The name of the table. For single files it is best to name the
     *   table the same as the file category.  For paired data it is best
     *   to use a name that represents both categoires.
     * @param options
     *   An associative array that contains the following keys:
     *   table_id: The HTML id of the table.  For single data, the table
     *     must already have 4 columns with headers (file name,
     *     size, progress and action). For paired data, the table
     *     must already have 8 columns, which are the same as the
     *     single table but with two sets.
     *   category:  An array. It must contain the list of categories that
     *     this table manages.  For paired data include two categories.
     *     This is the category of the file when saved in Tripal.
     *   submit_id: The HTML id of the submit button.
     *   module: The name of the module managing the table.
     *   cardinatily:  (optional) The number of files allowed.  Set to 0 for 
     *     unlimited.  Defalt is 0.
     *   target_id: (optional). The HTML id of the hidden field in the form 
     *     where the file ID will be written to this field. This only 
     *     works if cardinality is set to 1.
     *   allowed_types: (optional). An array of allowed file extensions (e.g.
     *     fasta, fastq, fna, gff3, etc.).
     */
    this.addUploadTable = function(tname, options) {
      var table_id = options['table_id'];
      var categories = options['category'];
      var submit_id = options['submit_id'];
      var target_id = options['target_id'];
      var cardinality = options['cardinality'];
      var module = options['module'];
      var allowed_types = options['allowed_types'];
      
      // Save the table ID for this category
      if (!(tname in this.tables)) {
        this.tables[tname] = {};
      }
      this.tables[tname]['table_id'] = table_id;
      this.tables[tname]['category'] = categories;
      this.tables[tname]['submit_id'] = submit_id;
      this.tables[tname]['target_id'] = target_id;
      this.tables[tname]['cardinality'] = cardinality;
      this.tables[tname]['module'] = module;
      this.tables[tname]['allowed_types'] = allowed_types;
      this.updateTable(categories[0]);
      this.enableSubmit(submit_id);
    }
    
    /**
     * Adds a click event to the submit button that starts the upload.
     */
    this.enableSubmit = function(submit_id) {
      var self = this;
      var categories = [];
      
      // Iterate through all of the tables that use this submit button
      // and collect all the categories.  We want to update them all.
      for (var tname in this.tables) {
        if (this.tables[tname]['submit_id'] == submit_id){
          for (var i = 0; i < this.tables[tname]['category'].length; i++) {
            categories.push(this.tables[tname]['category'][i])
          } 
        }
      }
      var func_name = ($.isFunction($.fn.live)) ? 'live' : 'on';
      $(submit_id)[func_name]('click', function() {
        for(var i = 0; i < categories.length; i++) {
          self.start(categories[i]);
        }
      });
    }
    
    /**
     * Updates the table for the given file category.
     */
    this.updateTable = function(category) {
      // Iterate through all of the tables that are managed by this object.
      for (var tname in this.tables) {
        // Iterate through all of the categories on each table.
        for (var i = 0; i < this.tables[tname]['category'].length; i++) {
          // If the category of the table matches then update it.
          if (this.tables[tname]['category'][i] == category) {
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
    this.setTarget = function(tname) {
      var categories = this.tables[tname]['category'];
      var num_categories = categories.length;
      var cardinality = this.tables[tname]['cardinality'];
      var target_id = this.tables[tname]['target_id'];
      
      if (target_id) {
        var fids = [];
        var c;

        // Iterate through the file categories.
        for (c = 0; c < num_categories; c++) {
          var files  = this.getCategoryFiles(categories[c]);
          var num_files = this.getNumFiles(categories[c]);
          var cat_fids = [];

          $.each(files, function(idx, file) {
            cat_fids.push(file.file_id);
          });
          fids.push(cat_fids.join('|'));
        }
        $('#' + target_id).val(fids.join(','));
      }
    }

    /**
     *
     */
    this.updateTableHTML = function(tname, categories) {
      if (typeof(categories) != "object") {
        categories = [categories];
      }

      var max_rows_allowed = this.tables[tname]['cardinality'];
      var table_id = this.tables[tname]['table_id'];
      var content = '';
      var buttons = [];

      var indexes = {};
      var row_has_file, row, row_buttons;
      var highest_index = 0;

      for (var cat_idx in categories) {
        for (var file_idx in this.getCategoryFiles(categories[cat_idx])) {
          indexes[file_idx] = file_idx;
          highest_index = ((file_idx > highest_index) ? file_idx : highest_index);
        }
      }
      var rows_with_files = Object.keys(indexes).length;

      for (var idx in indexes) {
        [row_has_file, row, row_buttons] = this.getRowHTML(idx, tname, categories)
        if (row_has_file) {
          content += row;
          buttons = buttons.concat(row_buttons);
        }
      }

      if (!max_rows_allowed || max_rows_allowed == 0 || max_rows_allowed > rows_with_files) {
        [row_has_file, row, row_buttons] = this.getRowHTML(highest_index + 1, tname, categories)
        content += row;
        buttons = buttons.concat(row_buttons);
      }

      $(table_id + ' > tbody').html(content);
      for (var i in buttons) {
        this.enableFileButton(buttons[i]['name']);
      }
    }

    /**
     *
     */
    this.getRowHTML = function(rownum, tname, categories) {
      var row_buttons = [];
      var row = '<tr class="' + ((rownum % 2) ? 'even' : 'odd') + '">';
      var row_has_file = false;

      if (typeof(categories) != "object") {
        categories = [categories];
      }

      for (var cat of categories) {
        var file = this.getCategoryFile(cat, rownum);
        if (file) {
          row += '<td>' + file.getFileName() + '</td>';
          row += '<td>' + file.getFileSize(true)  + '</td>';
          row += '<td>' + file.getProgressBar() + '</td>';
          row += '<td>' + file.getLinks() + '</td>';
          row_has_file = true;
        }
        else {
          var button = this.getFileButton(tname, cat, rownum);
          row_buttons.push(button);
          row += '<td colspan="4">' + button['element'] + '</td>';
        }
      }
      row += '</tr>';

      return [row_has_file, row, row_buttons];
    }

    /**
     * Adds a function to the change event for the file button that
     * causes a new file to be added to this object which it is clicked.
     * The button is added by the updateUploadTable
     */
    this.enableFileButton = function(button_name) {
     
      // If the button already exists then it's already setup so just
      // return.
      if($('#' + button_name).attr('ready') == 'true') {
        return;
      }


      // When the button provided by the TripalUploader class is clicked
      // then we need to add the files to the object.  We must have this
      // function so that we can set the proper URL
      var self = this;

      var func_name = ($.isFunction($.fn.live)) ? 'live' : 'on';
      $('#' + button_name)[func_name]('change', function(e) {
        var id = this.id;
        
        // Get the HTML5 list of files to upload.
        var hfiles = e.target.files;

        // Let the TripalUploader object parse the button ID to give us
        // the proper category name and index.
        var button = self.parseButtonID(id);
        var tname = button['tname'];
        var category = button['category'];
        var index = button['index'];

        // Add the file(s) to the uploader object.
        for (var i = 0; i < hfiles.length; i++) {
          var f = hfiles[i];
          var baseurl = window.location.protocol + '//' + window.location.host + drupalSettings.path.baseUrl;
          var options = {
            // Files are managed by tables.
            'tname' : tname,
            // Files can be categorized to seprate them from other files.
            'category': category,
            // The index is the numeric index of the file. Files are ordered
            // by their index. The file with an index of 0 is always ordered first.
            'i': index,
            // The URL at the remote server where the file will uploaded. 
            'url' : baseurl + 'tripal/upload/' + category,
            };
            self.addFile(f, options);
 
            // We need to update the upload table and the progress. The
          // information for which table to update is in the self.tables
          // array.
          self.updateTable(category);
        }
      });
      $('#' + button_name).attr('ready', 'true');
    }
  };

  // Export the objects to the window for use in other JS files.
  window.TripalUploader = TripalUploader;

})(jQuery);