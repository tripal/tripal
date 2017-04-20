<?php
class TripalImporter {
  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Tripal Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'tripal_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'A base loader for all Tripal loaders';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = array();


  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = '';

  /**
   * The title that should appear above the upload button.
   */
  public static $upload_title = 'File Upload';


  // --------------------------------------------------------------------------
  //                          CONSTRUCTORS
  // --------------------------------------------------------------------------
  /**
   * Instantiates a new TripalImporter object.
   *
   * @param $job_id
   */
  public function __construct() {

  }

  // --------------------------------------------------------------------------
  //                     PROTECTED FUNCTIONS
  // --------------------------------------------------------------------------


  // --------------------------------------------------------------------------
  //                     OVERRIDEABLE FUNCTIONS
  // --------------------------------------------------------------------------

  public function form($form, &$form_state) {

    return $form;
  }
  public function formSubmit($form, &$form_state) {

  }
  public function formValidate($form, &$form_state) {

  }
  public function preRun($details) {

  }
  public function run($details, $job_id = NULL) {

  }
  public function postRun($details) {

  }

}