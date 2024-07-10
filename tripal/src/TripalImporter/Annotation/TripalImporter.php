<?php

namespace Drupal\tripal\TripalImporter\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a tripal importer annotation object.
 *
 * @see \Drupal\products\Plugin\ImporterManager
 *
 * @Annotation
 */
class TripalImporter extends Plugin {

  /**
   * The plugin ID.
   *
   * @var string
   */
  public $id;

  /**
   * The label of the plugin.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $label;

  /**
   * A brief description for this loader.
   *
   * This description will be presented to the site user.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $description;

  /**
   * An array containing the extensions of allowed file types.
   *
   * @var array
   */
  public $file_types = ['txt'];

  /**
   * Provides information to the user about the file upload.
   *
   * Typically this may include a description of the file types allowed.
   */
  public $upload_description;

  /**
   * The title that should appear above the upload button.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $upload_title;

  /**
   * If the loader should require an analysis record.
   *
   * To maintain provenance we should always indicate where the data we are
   * uploading comes from. The method that Tripal attempts to use for this
   * by associating upload files with an analysis record.  The analysis
   * record provides the details for how the file was created or obtained.
   * Set this to FALSE if the loader should not require an analysis when
   * loading. if $use_analysis is set to true then the form values will
   * have an 'analysis_id' key in the $form_state array on submitted forms.
   *
   * @var bool
   */
  public $use_analysis = TRUE;

  /**
   * If the $use_analysis value is set above then this value indicates if the
   * analysis should be required.
   *
   * @var bool
   */
  public $require_analysis = FALSE;

  /**
   * Indicates whether the base importer should add a submit button or not.
   * This should only be used in situations were you need multiple buttons
   * or control over the submit process (e.g. multi-page forms).
   *
   * @var bool
   */
  public $use_button = TRUE;

  /**
   * Indicated whether the base importer added submit button should be disabled
   * when the form is first loaded (i.e when the user clicks the link for the
   * importer). The form can then be programatically enabled via AJAX once
   * certain criteria is set by setting the form state storage.
   *
   * Example of programatically enabling the button via the form state
   * on an importer where this annotation is set to TRUE.
   * @code
      $storage = $form_state->getStorage();
      $storage['disable_TripalImporter_submit'] = FALSE;
      $form_state->setStorage($storage);
   * @endcode
   */
  public $submit_disabled = FALSE;

  /**
   * Text that should appear on the button at the bottom of the importer form.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $button_text = 'Import';

  /**
   * Indicates if the loader should provide a file upload form element.
   *
   * @var bool
   */
  public $file_upload = TRUE;

  /**
   * Indicates if the loader should provide a local file form element.
   *
   * @var bool
   */
  public $file_local = TRUE;

  /**
   * Indicates if the loader should provide a remote file form element.
   *
   * @var bool
   */
  public $file_remote = TRUE;

  /**
   * Indicates if the file must be provided.
   *
   * An example when it may not be
   * necessary to require that the user provide a file for uploading if the
   * loader keeps track of previous files and makes those available for
   * selection.
   *
   * @var bool
   */
  public $file_required = TRUE;

  /**
   * The array of arguments used for this loader.
   *
   * Each argument should
   * be a separate array containing a machine_name, name, and description
   * keys.  This information is used to build the help text for the loader.
   *
   * @var array
   */
  public $argument_list = [];

  /**
   * Indicates how many files are allowed to be uploaded.
   *
   * A value of zero indicates an unlimited number of uploaded files
   * are allowed.
   *
   * @var int
   */
  public $cardinality = 1;

  /**
   * Be default, all loaders are automaticlly added to the Admin >
   * Tripal > Data Loaders menu.  However, if this loader should be
   * made available via a different menu path, then set it here.  If the
   * value is empty then the path will be the default.
   *
   * @var string
   */
  public $menu_path;

  /**
   * If your importer requires more flexibility and advanced features than
   * the TripalImporter provides, you can indicate a callback function. If set,
   * the callback will be used to provide the importer interface to the
   * end-user.  However, because this bypasses the class infrastructure the
   * run() function will also not be available and your importer must be
   * fully self-sufficient outside of this class.  The benefit for using a
   * TripalImporter despite your loader being self-sufficient is that Tripal
   * will treat your loader like all others providing a consistent location
   * in the menu and set of permissions.
   *
   * Note: use of a callback is discouraged as the importer provides a
   * consistent workflow for all importers.  Try your best to fit your importer
   * within the class.  Use this if you absolutely cannot fit your importer
   * into  TripalImporter implementation.
   *
   * @var string
   */
  public $callback;

  /**
   * The name of the module that provides the callback function.
   *
   * @var string
   */
  public $callback_module;

  /**
   * An include path for the callback function.  Use a relative path within
   * this scope of this module
   * (e.g. includes/loaders/tripal_chado_pub_importers).
   *
   * @var string
   */
  public $callback_path;

}
