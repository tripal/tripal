(function (Drupal, once) {

  Drupal.behaviors.TripalFile = {
    attach(context, settings) {

      const tripalFiles = new Drupal.TripalUploader();

      once(
        'tripal-file-init',
        '.tripal-html5-file-upload-table-key',
        context
      ).forEach((element) => {

        const id = element.value;

        if (!id.trim()) {
          return;
        }
        const [usage_id, usage_type, module] = id.split('-');

        const settingsVarName =
          `uploader_${usage_id}_${usage_type}_${module}`;

        if (settings.tripal?.[settingsVarName]) {
          tripalFiles.addUploadTable(
        if (settings.tripal?.[settingsVarName]) {
          tripalFiles.addUploadTable(
            `${usage_id}-${usage_type}`,
            settings.tripal[settingsVarName]
          );
        }
      });
    }
  };

})(Drupal, once);
