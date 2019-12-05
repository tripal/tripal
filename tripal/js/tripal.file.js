(function ($) {
  Drupal.behaviors.TripalFile = {
    attach: function (context, settings) {
      // Initialize the TripalUploader object.
      var tripal_files = new TripalUploader();

      // All tables that belong to the html5-file form element should
      // be enabled for uploading.
      $('.tripal-html5-file-upload-table-key').each(function (index) {
        // If we already attached functionality to the field, skip it
        if ($(this).data('tripal.file')) {
          return;
        }

        // Set the field status
        $(this).data('tripal.file', true);

        // The settings for this uploader are provided in a custom variable
        // specific to the table. We can get the variable name by piecing
        // together parts of the table ID.
        var id                = $(this).val();
        var details           = id.split('-');
        var settings_var_name = 'Drupal.settings.uploader_' + details[0] + '_' + details[1] + '_' + details[2];
        var settings          = eval(settings_var_name);

        // Initialize the table for uploads.
        tripal_files.addUploadTable(details[0] + '-' + details[1], settings);
      });
    }
  };
})(jQuery);