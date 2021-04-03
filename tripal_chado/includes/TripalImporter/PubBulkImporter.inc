<?php

class PubBulkImporter extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Chado Bulk Publication Importer';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_pub_bulk';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Create and modify importers that can connect to and retrieve publications from remote databases.';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = [];


  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = '';

  /**
   * The title that should appear above the upload button.
   */
  public static $upload_title = 'File Upload';

  /**
   * If the loader should require an analysis record.  To maintain provenance
   * we should always indicate where the data we are uploading comes from.
   * The method that Tripal attempts to use for this by associating upload files
   * with an analysis record.  The analysis record provides the details for
   * how the file was created or obtained. Set this to FALSE if the loader
   * should not require an analysis when loading. if $use_analysis is set to
   * true then the form values will have an 'analysis_id' key in the $form_state
   * array on submitted forms.
   */
  public static $use_analysis = FALSE;

  /**
   * If the $use_analysis value is set above then this value indicates if the
   * analysis should be required.
   */
  public static $require_analysis = FALSE;

  /**
   * Text that should appear on the button at the bottom of the importer
   * form.
   */
  public static $button_text = 'Import';

  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => FALSE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => FALSE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => FALSE,
  ];

  /**
   * Indicates if the file must be provided.  An example when it may not be
   * necessary to require that the user provide a file for uploading if the
   * loader keeps track of previous files and makes those available for
   * selection.
   */
  public static $file_required = FALSE;


  /**
   * The array of arguments used for this loader.  Each argument should
   * be a separate array containing a machine_name, name, and description
   * keys.  This information is used to build the help text for the loader.
   */
  public static $argument_list = [];


  /**
   * Indicates how many files are allowed to be uploaded.  By default this is
   * set to allow only one file.  Change to any positive number. A value of
   * zero indicates an unlimited number of uploaded files are allowed.
   */
  public static $cardinality = 0;

  /**
   * Be default, all loaders are automatically added to the Admin >
   * Tripal > Data Loaders menu.  However, if this loader should be
   * made available via a different menu path, then set it here.  If the
   * value is empty then the path will be the default.
   */
  public static $menu_path = '';

  /**
   * If your importer requires more flexibility and advance features than
   * the TripalImporter provides you can indicate a callback function. If set,
   * the callback will be used to provide the importer interface to the
   * end-user.  However, because this bypasses the class infrastructure the
   * run() function will also not be available and your importer must be
   * fully self-sufficient outside of this class.  The benefit for using a
   * TripalImporter despite your loader being self-sufficient is that Tripal
   * will treat your loader like all others providing a consistent location
   * in the menu and set of permissions.
   */
  public static $callback = 'tripal_pub_importers_list';

  /**
   * The name of the module that provides the callback function.
   */
  public static $callback_module = 'tripal_chado';

  /**
   * An include path for the callback function.  Use a relative path within
   * this scope of this module
   * (e.g. includes/loaders/tripal_chado_pub_importers).
   */
  public static $callback_path = 'includes/loaders/tripal_chado.pub_importers.inc';

}
