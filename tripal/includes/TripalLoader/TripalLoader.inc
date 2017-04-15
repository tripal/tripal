<?php
class TripalLoader {
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
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'A base loader for all Tripal loaders';


  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  /**
   * The loader can be executed as a Tripal job, in that case we want
   * to keep track of the job record.
   */
  protected $job;



  // --------------------------------------------------------------------------
  //                          CONSTRUCTORS
  // --------------------------------------------------------------------------
  /**
   * Instantiates a new TripalLoader object.
   *
   * @param $job_id
   */
  public function __construct($job_id = NULL) {
    $this->job = NULL;
    if ($job_id) {
      $this->job = tripal_get_job($job_id);
    }
  }

  // --------------------------------------------------------------------------
  //                     PROTECTED FUNCTIONS
  // --------------------------------------------------------------------------

  protected function setJobProgress($percentage) {
     if (!is_numeric($percent_complete)) {
       throw new Exception('TripalLoader: Percent complete must be numeric.');
     }
     if (!$this->job) {
       throw new Exception('TripalLoader: This loader is not associated with a job. Cannot set the job progress.');
     }

     tripal_set_job_progress($this->job->job_id, $percentage);
  }

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
  public function preRun() {

  }
  public function run() {
     if (!$this->job) {
       throw new Exception('TripalLoader: This loader is not associated with a job. Cannot run the loader.');
     }
  }
  public function postRun() {

  }

}