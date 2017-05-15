(function($) {
  Drupal.behaviors.TripalFile = {
    attach: function (context, settings) {

      var tripal_files = new TripalUploader();
      
      $(".tripal-html5-file-upload-table-key").each(function(index) {
        var form_key = $(this).val()
        tripal_files.addUploadTable(form_key, {
          'table_id' : '#tripal-html5-file-upload-table-' + form_key,
          'submit_id': '#tripal-html5-file-upload-submit-' + form_key,
          'category' : [form_key],
          'cardinality' : 1,
          'target_id' : 'tripal-html5-upload-fid-' + form_key,
          'module' : 'tripal_core',
        });
      });
    }
  }
}) (jQuery);
