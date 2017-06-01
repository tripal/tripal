(function($) {
  Drupal.behaviors.TripalFile = {
    attach: function (context, settings) {

      var tripal_files = new TripalUploader();
      
      $(".tripal-html5-file-upload-table-key").each(function(index) {
        var form_key = $(this).val()
        var id = $(this).val()
        var details = id.split("-");
        var form_key = details[0] + '-' + details[1];
        var module = details[2];

        tripal_files.addUploadTable(form_key, {
          'table_id' : '#tripal-html5-file-upload-table-' + id,
          'submit_id': '#tripal-html5-file-upload-submit-' + id,
          'category' : [form_key],
          'cardinality' : 1,
          'target_id' : 'tripal-html5-upload-fid-' + id,
          'module' : module,
        });
      });
    }
  }
}) (jQuery);
