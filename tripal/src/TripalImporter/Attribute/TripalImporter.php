<?php

namespace Drupal\tripal\TripalImporter\Attribute;

use Drupal\Component\Plugin\Attribute\Plugin;
use Drupal\Core\StringTranslation\TranslatableMarkup;

/**
 * Defines a Tripal Importer attribute object.
 *
 * Plugin Namespace: Drupal\tripal\TripalImporter.
 */
#[\Attribute(\Attribute::TARGET_CLASS)]
class TripalImporter extends Plugin {

  /**
   * Constructs a Tripal Importer attribute.
   *
   * @param string $id
   *   The plugin ID.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $label
   *   The label of the importer plugin.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $description
   *   A brief description for this loader.
   *   This description will be presented to the site user.
   * @param array|null $file_types
   *   An array containing the extensions of allowed file types.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $upload_description
   *   Provides information to the user about the file upload.
   *   Typically this may include a description of the file types allowed.
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $upload_title
   *   The title that should appear above the upload button.
   * @param bool|null $use_analysis
   *   If the loader should require an analysis record.
   *   To maintain provenance we should always indicate where the data we are
   *   uploading comes from. The method that Tripal attempts to use for this
   *   by associating upload files with an analysis record. The analysis
   *   record provides the details for how the file was created or obtained.
   *   Set this to FALSE if the loader should not require an analysis when
   *   loading. if $use_analysis is set to true then the form values will
   *   have an 'analysis_id' key in the $form_state array on submitted forms.
   * @param bool|null $require_analysis
   *   If the $use_analysis value is set above then this value indicates if the
   *   analysis should be required.
   * @param bool|null $use_button
   *   Indicates whether the base importer should add a submit button or not.
   *   This should only be used in situations were you need multiple buttons
   *   or control over the submit process (e.g. multi-page forms).
   * @param bool|null $submit_disabled
   *   Used to disabled the base importer added submit button when the form
   *   is first loaded (i.e when the user clicks the link for the importer). 
   *   The form can then be programatically enabled via AJAX by setting the 
   *   form state storage as shown in the following example.
   *   @code
   *     $storage = $form_state->getStorage();
   *     $storage['disable_TripalImporter_submit'] = FALSE;
   *     $form_state->setStorage($storage);
   *   @endcode
   * @param \Drupal\Core\StringTranslation\TranslatableMarkup|null $button_text
   *   Text that should appear on the button at the bottom of the importer form.
   * @param bool|null $file_upload
   *   Indicates if the loader should provide a file upload form element.
   * @param bool|null $file_local
   *   Indicates if the loader should provide a local file form element.
   * @param bool|null $file_remote
   *   Indicates if the loader should provide a remote file form element.
   * @param bool|null $file_required
   *   Indicates if the file must be provided.
   *   An example when it may not be
   *   necessary to require that the user provide a file for uploading if the
   *   loader keeps track of previous files and makes those available for
   *   selection.
   * @param array|null $publish
   *   An associative array indicating what Tripal Content should be published
   *   after import. It is keyed by the list type and the value indicates what
   *   to publish using that approach. List Types supported:
   *   - bundle (array): a list of bundle machine names to publish.
   * @param bool|null $hidden
   *   Indicates if the importer will not appear in the importer menu.
   *   This is used by the publication importer because it is more complicated
   *   than the base importer class supports.
   * @param array|null $argument_list
   *   The array of arguments used for this loader.
   *   Each argument should be a separate array containing a machine_name,
   *   name, and description keys. This information is used to build the help
   *   text for the loader.
   * @param int|null $cardinality
   *   Indicates how many files are allowed to be uploaded.
   *   A value of zero indicates an unlimited number of uploaded files
   *   are allowed.
   * @param string|null $menu_path
   *   By default, all loaders are automaticlly added to the Admin >
   *   Tripal > Data Loaders menu.  However, if this loader should be
   *   made available via a different menu path, then set it here.  If the
   *   value is empty then the path will be the default.
   * @param string|null $callback
   *   If your importer requires more flexibility and advanced features than
   *   the TripalImporter provides, you can indicate a callback function. If
   *   set, the callback will be used to provide the importer interface to the
   *   end-user. However, because this bypasses the class infrastructure the
   *   run() function will also not be available and your importer must be
   *   fully self-sufficient outside of this class. The benefit for using a
   *   TripalImporter despite your loader being self-sufficient is that Tripal
   *   will treat your loader like all others providing a consistent location
   *   in the menu and set of permissions.
   *   Note: use of a callback is discouraged as the importer provides a
   *   consistent workflow for all importers. Try your best to fit your
   *   importer within the class. Use this if you absolutely cannot fit your
   *   importer into TripalImporter implementation.
   * @param string|null $callback_path
   *   An include path for the callback function. Use a relative path within
   *   the scope of this module
   *   (e.g. includes/loaders/tripal_chado_pub_importers).
   */
  public function __construct(
    public readonly string $id,
    public readonly ?TranslatableMarkup $label = NULL,
    public readonly ?TranslatableMarkup $description = NULL,
    public readonly ?array $file_types = ['txt'],
    public readonly ?TranslatableMarkup $upload_description = NULL,
    public readonly ?TranslatableMarkup $upload_title = NULL,
    public readonly ?bool $use_analysis = TRUE,
    public readonly ?bool $require_analysis = FALSE,
    public readonly ?bool $use_button = TRUE,
    public readonly ?bool $submit_disabled = FALSE,
    public readonly ?TranslatableMarkup $button_text = new TranslatableMarkup('Import'),
    public readonly ?bool $file_upload = TRUE,
    public readonly ?bool $file_local = TRUE,
    public readonly ?bool $file_remote = TRUE,
    public readonly ?bool $file_required = TRUE,
    public readonly ?array $publish = [],
    public readonly ?bool $hidden = FALSE,
    public readonly ?array $argument_list = [],
    public readonly ?int $cardinality = 1,
    public readonly ?string $menu_path = '',
    public readonly ?string $callback = '',
    public readonly ?string $callback_path = '',
  ) {}

}
