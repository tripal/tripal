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
        const details = id.split('-');

        const settingsVarName =
          `uploader_${details[0]}_${details[1]}_${details[2]}`;

        if (settings.tripal?.[settingsVarName]) {
          tripalFiles.addUploadTable(
            `${details[0]}-${details[1]}`,
            settings.tripal[settingsVarName]
        if (settings.tripal?.[settingsVarName]) {
          tripalFiles.addUploadTable(
            `${usage_id}-${usage_type}`,
            settings.tripal[settingsVarName]
        }
      });
    }
  };

})(Drupal, once);
