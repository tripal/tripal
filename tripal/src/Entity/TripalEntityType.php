<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;

/**
 * Defines the Tripal Content type entity.
 *
 * @ConfigEntityType(
 *   id = "tripal_entity_type",
 *   label = @Translation("Tripal Content Type"),
 *   label_collection = @Translation("Tripal Content Types"),
 *   label_singular = @Translation("Tripal content type"),
 *   label_plural = @Translation("Tripal content types"),
 *   label_count = @PluralTranslation(
 *     singular = "@count Tripal content type",
 *     plural = "@count Tripal content types",
 *   ),
 *   handlers = {
 *     "list_builder" = "Drupal\tripal\ListBuilders\TripalEntityTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\tripal\Form\TripalEntityTypeForm",
 *       "edit" = "Drupal\tripal\Form\TripalEntityTypeForm",
 *       "delete" = "Drupal\tripal\Form\TripalEntityTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\tripal\Routing\TripalEntityTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "content_type",
 *   admin_permission = "manage tripal content types",
 *   bundle_of = "tripal_entity",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/bio_data/{tripal_entity_type}",
 *     "add-form" = "/admin/structure/bio_data/add",
 *     "edit-form" = "/admin/structure/bio_data/manage/{tripal_entity_type}",
 *     "delete-form" = "/admin/structure/bio_data/manage/{tripal_entity_type}/delete",
 *     "collection" = "/admin/structure/bio_data"
 *   },
 *   config_export = {
 *     "id",
 *     "label",
 *     "termIdSpace",
 *     "termAccession",
 *     "help_text",
 *     "category",
 *     "title_format",
 *     "url_format",
 *     "hide_empty_field",
 *     "ajax_field"
 *   }
 * )
 */
class TripalEntityType extends ConfigEntityBundleBase implements TripalEntityTypeInterface {

  /**
   * The Tripal Content type ID.
   *
   * @var integer
   */
  protected $id;

  /**
   * The Tripal Content type label.
   *
   * @var string
   */
  protected $label;

  /**
   * The Tripal Term ID Space which describes this content type.
   *
   * @var string
   */
  protected $termIdSpace;

  /**
   * The Tripal Term Accession which describes this content type.
   *
   * @var string
   */
  protected $termAccession;

  /**
   * Help text to describe to the administrator what this content type is.
   *
   * @var string
   */
  protected $help_text;

  /**
   * The category the given content type belongs to.
   *
   * @var string
   */
  protected $category;

  /**
   * The format for titles including tokens.
   *
   * @var string
   */
  protected $title_format;

  /**
   * The format for the url alias including tokens.
   *
   * @var string
   */
  protected $url_format;

  /**
   * Indicates that empty fields should be hidden.
   *
   * @var boolean
   */
  protected $hide_empty_field;

  /**
   * Indicates that AJAX should be used to load fields.
   *
   * @var boolean
   */
  protected $ajax_field;

  // --------------------------------------------------------------------------
  //                             CREATE + SAVE
  //
  // Tripal Entity Types are created using the Drupal Entity API.
  // To create new type, you will use the static create function we extended
  // below to pass in the info for the new type and then call the save method on
  // the created object to permanently save it to your site.
  //
  //    $new_entityType = TripalEntityType::create($details);
  //    if (is_object($entityType)) {
  //      $new_entityType->save();
  //    }
  // --------------------------------------------------------------------------

  /**
   * Contructs a new TripalEntityType object without permanently saving it.
   *
   * Extends EntityBase::create() with support for TripalTerm as a value.
   *
   * @param array $values
   *    An array of values to set, keyed by property name. Supported keys are:
   *      -
   * @return TripalEntityType
   *    An TripalEntityType with the values passed in set appropriately.
   */
  public static function create(array $values = []) {

    // Check if a TripalTerm object was passed in.
    // If yes, extract the ID Space and Accession for saving.
    if (array_key_exists('term', $values)) {
      if (is_a($values['term'], '\Drupal\tripal\TripalVocabTerms\TripalTerm')) {
        $term = $values['term'];
        unset($values['term']);
        $values['termIdSpace'] = $term->getIdSpace();
        $values['termAccession'] = $term->getAccession();

        // Since we have a term object, we can use the definition to set the
        // help text if it's not already set.
        if (!array_key_exists('help_text', $values)) {
          $values['help_text'] = $term->getDefinition();
        }
      }
      else {
        $class_name_passed_in = get_class($values['term']);
        throw new \Exception("When passing a term to create a TripalEntityType is must be of type TripalTerm. You passed in an object of type " . $class_name_passed_in . ".");
      }
    }

    // Let the parent implementation finish creating the object.
    // NOTE: We do things in this order because a configuration entity cannot
    // save an object to its storage. Thus we need to extract the term strings
    // for storage and retrieve the Term object later if requested via getTerm().
    return parent::create($values);
  }


  /**
   * Saves the new TripalEntityType permanently.
   *
   * Extends ConfigEntityBase::save() with support for creating the associated
   * TripalTerm if it doesn't already exist.
   *
   * When saving existing entities, the entity is assumed to be complete,
   * partial updates of entities are not supported.
   *
   * @return int
   *   Either SAVED_NEW or SAVED_UPDATED, depending on the operation performed.
   */
   public function save() {

     // Set defaults for anything not already set.
     $this->setDefaults();

     // Validate the values before trying to save.
     $this->validate();

     // Save the rest of the entity using the parent implementation.
     // This is when the id is assigned.
     $return_status = parent::save();

     return $return_status;
   }

  /**
   * Validate the expected values before saving.
   *
   * Note: This function throws exceptions so make sure to catch them ;-p
   * We do not want users seeing a WSOD.
   */
  public function validate() {

    if ($this->label === NULL) {
      throw new \Exception("The label is required when creating a TripalEntityType.");
    }
    if ($this->id === NULL) {
      throw new \Exception("The id is required when creating a TripalEntityType.");
    }
    if ($this->help_text === NULL) {
      throw new \Exception("The help text is required when creating a TripalEntityType.");
    }
    if ($this->termIdSpace === NULL) {
      throw new \Exception("The Term ID Space is required when creating a TripalEntityType.");
    }
    if ($this->termAccession === NULL) {
      throw new \Exception("The Term Accession is required when creating a TripalEntityType.");
    }

    // Check that the TripalTerm exists with the ID Space and Accession
    // added to this type when it was created.

    // If not, then create the TripalTerm, TripalIDSpace and TripalVocabulary.
  }

  // --------------------------------------------------------------------------
  //                          MAIN SETTER / GETTERS
  //
  // The following methods allow the main properties of the Tripal Entity Type
  // to be set or retrieved. These properties include ID, macine name, term
  // help text and category.
  // --------------------------------------------------------------------------

  /**
   * Set defaults of values which are not yet set.
   */
  public function setDefaults() {

    if ($this->category === NULL) {
      $this->category = 'General';
    }
    if ($this->hide_empty_field === NULL) {
      $this->hide_empty_field = TRUE;
    }
    if ($this->ajax_field === NULL) {
      $this->ajax_field = TRUE;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function id() {
    return $this->id;
  }

  /**
   * {@inheritdoc}
   */
  public function getID() {
    return $this->id;
  }

  /**
   * {@inheritdoc}
   */
  public function getLabel() {
    return $this->label;
  }

  /**
   * {@inheritdoc}
   */
  public function setLabel($label) {
    $this->label = $label;
  }

  /**
   * {@inheritdoc}
   */
  public function getTermIdSpace() {
    return $this->termIdSpace;
  }

  /**
   * {@inheritdoc}
   */
  public function setTermIdSpace($termIdSpace) {
    $this->termIdSpace = $termIdSpace;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getTermAccession() {
    return $this->termAccession;
  }

  /**
   * {@inheritdoc}
   */
  public function setTermAccession($termAccession) {
    $this->termAccession = $termAccession;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getTerm() {
    $manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idspace = $manager->loadCollection($this->termIdSpace);
    if (is_object($idspace)) {
      return $idspace->getTerm($this->termAccession);
    }
    else {
      return NULL;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function setTerm(TripalTerm $term) {
    $this->termIdSpace = $term->getIdSpace();
    $this->termAccession = $term->getAccession();

    // Since we have a term object, we can use the definition to set the
    // help text if it's not already set.
    if ($this->help_text === NULL) {
      $this->help_text = $term->getDefinition();
    }
  }

  /**
   * {@inheritdoc}
   */
  public function getHelpText() {
    return $this->help_text;
  }

  /**
   * {@inheritdoc}
   */
  public function setHelpText($help_text) {
    $this->help_text = $help_text;
  }

  /**
   * {@inheritdoc}
   */
  public function getCategory() {
    if ($this->category) {
      return $this->category;
    }
    else {
      return 'General';
    }
  }

  /**
   * {@inheritdoc}
   */
  public function setCategory($category) {
    $this->category = $category;
  }

  // --------------------------------------------------------------------------
  //                              TITLE FORMATS
  //
  // The following methods all pertain to setting titles for Tripal Content
  // Pages. Specifically, curators can set the title of a specific page or
  // allow the default pattern to generate the title. The pattern is specified
  // for all pages of a given Tripal Entity Type and is known as a
  // "Title Format" and can be set when the type is created or edited.
  // --------------------------------------------------------------------------

  /**
   * {@inheritdoc}
   */
  public function getTitleFormat() {
    if ($this->title_format) {
      return $this->title_format;
    }
    else {
      return $this->getDefaultTitleFormat();
    }
  }

  /**
   * {@inheritdoc}
   */
  public function setTitleFormat($title_format) {
    $this->title_format = $title_format;
  }

  /**
   * {@inheritdoc}
   * @todo add to docs
   */
  public function getDefaultTitleFormat() {

    // Retrieve all available tokens.
    $tokens = $this->getTokens([
      'include id' => FALSE,
      'include title' => FALSE,
    ]);

    // Set an extremely ugly empty title for in case there are no tokens/fields.
    $format = 'Unknown ' . date('Ymd-h:i:sA');

    // A) Check to see if more informed modules have suggested a title for this
    //    type. Invoke hook_tripal_default_title_format() to get all suggestions
    //    from other modules.
    $suggestions = \Drupal::moduleHandler()->invokeAll(
      'tripal_default_title_format',
      [$this, $tokens]
    );
    if ($suggestions) {
      // Use the suggestion with the lightest weight.
      $lightest_key = NULL;
      foreach ($suggestions as $k => $s) {
        if ($lightest_key === NULL) {
          $lightest_key = $k;
        }
        if ($s['weight'] < $lightest_key) {
          $lightest_key = $k;
        }
      }
      $format = $suggestions[$lightest_key]['format'];
    }
    // B) Generate our own ugly title by simply comma-separating all the
    //    required fields.
    else {
      $tmp = [];

      // Check which tokens are required fields and join them into a default
      // format.
      if (sizeof($tokens) > 0) {
        foreach ($tokens as $token) {

          // Exclude the type & term since it is not unique.
          if ($token['token'] == '[type]') {
            continue;
          }

          // If it is required then add it to the default title
          // since we know it has a value.
          if ($token['required']) {
            $tmp[] = $token['token'];
          }
        }
        $format = implode(', ', $tmp);
      }
    }

    return $format;
  }

  // --------------------------------------------------------------------------
  //                             URL ALIAS FORMATS
  //
  // The following methods all pertain to setting aliases for Tripal Content
  // Pages. This allows administrators to set readable, more friendly URLs
  // for their biological content in bulk through the use of tokens and
  // patterns.
  // --------------------------------------------------------------------------

  /**
   * {@inheritdoc}
   */
  public function getURLFormat() {
    if ($this->url_format) {
      return $this->url_format;
    }
    else {
      return '[TripalEntityType__term_label]/[TripalEntity__entity_id]';
    }
  }

  /**
   * {@inheritdoc}
   */
  public function setURLFormat($url_format) {
    $this->url_format = $url_format;
  }

  // --------------------------------------------------------------------------
  //                                  TOKENS
  //
  // The following methods relate to Tripal Entity Type tokens. These tokens
  // are based on the fields for a given Tripal Entity Type and can be used
  // to indicate general patterns to allow bulk assignment of titles and URLs.
  // --------------------------------------------------------------------------

  /**
   * {@inheritdoc}
   * @todo add to docs.
   */
  public function getTokens($options = []) {

    $tokens = [];

    // Set default options.
    $options['required only'] = (isset($options['required only'])) ? $options['required only'] : FALSE;
    $options['include id'] = (isset($options['include id'])) ? $options['include id'] : TRUE;
    $options['include title'] = (isset($options['include title'])) ? $options['include title'] : TRUE;

    // ID Tokens:
    if ($options['include id'] == TRUE) {
      $token = '[TripalEntityType__entity_id]';
      $tokens[$token] = [
        'label' => 'Content Type/Bundle ID',
        'description' => 'The machine name for this Tripal Content Type. By default this will be similar to the label you entered. For example, if you created a content type with the label "Genome Annoation" then it\'s machine name/id would be "genome_annotation".',
        'token' => $token,
        'field_name' => NULL,
        'required' => TRUE,
      ];

      $token = '[TripalEntity__entity_id]';
      $tokens[$token] = [
        'label' => 'Content/Entity ID',
        'description' => 'The unique identifier for an individual piece of Tripal Content. This will be unique for each Tripal Content page and is an integer.',
        'token' => $token,
        'field_name' => NULL,
        'required' => TRUE,
      ];
    }

    // Term/Type Tokens:
    $token = '[TripalEntityType__label]';
    $tokens[$token] = [
      'label' => 'Tripal Entity Type',
      'description' => 'The human-readable label for this Tripal Content Type (e.g. "Genome Annotation").',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE,
    ];

    $token = '[TripalEntityType__term_namespace]';
    $tokens[$token] = [
      'label' => 'Content Type Term Namespace',
      'description' => 'The database name describing the term for this Tripal Content Type. For example, if this content type uses the term "gene (SO:0000704)" then the namespace is "SO".',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE,
    ];

    $token = '[TripalEntityType__term_accession]';
    $tokens[$token] = [
      'label' => 'Content Type Term Accession',
      'description' => 'The database accession describing the term for this Tripal Content Type. For example, if this content type uses the term "gene (SO:0000704)" then the accession is "0000704".',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE,
    ];

    $token = '[TripalEntityType__term_label]';
    $tokens[$token] = [
      'label' => 'Content Type Term Label',
      'description' => 'The human readable label of the term for this Tripal Content Type. For example, if this content type uses the term "gene (SO:0000704)" then the label is "gene".',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE,
    ];

    $instances = \Drupal::service('entity_field.manager')->getFieldDefinitions('tripal_entity', $this->id);
    foreach ($instances as $instance_name => $instance) {

      $use_field = TRUE;
      $field_name = $instance->getName();

      // Remove base fields.
      if (in_array($field_name, ['id', 'type', 'uid', 'term_id', 'title', 'status', 'created', 'changed'])) {
        continue;
      }

      // If only required fields should be returned,
      // skip this field if it's not required.
      if (!$instance->isRequired() and $options['required only']) {
        continue;
      }

      // Iterate through the TripalEntity fields and see if they have
      // sub-elements, if so, add those as tokens too.
      // @todo handle sub-elements once TripalField's are implemented.

      // If we have no elements to add then just add the field as is.
      if ($use_field) {
        // Build the token from the field information.
        $token = '[' . $field_name . ']';
        $tokens[$token] = [
          'label' => $instance->getLabel(),
          'description' => $instance->getDescription(),
          'token' => $token,
          'field_name' => $field_name,
          'required' => $instance->isRequired(),
        ];
      }
    }

    return $tokens;
  }

  // --------------------------------------------------------------------------
  //                             FIELD DISPLAY
  //
  // The following methods pertain to what fields are displayed and how they
  // are loaded. For example, administrators can choose to hide empty fields
  // or have all fields loaded by AJAX to speed up page loading times.
  // --------------------------------------------------------------------------

  /**
   * {@inheritdoc}
   */
  public function hideEmptyFields() {
    $this->hide_empty_field = TRUE;
  }

  /**
   * {@inheritdoc}
   */
  public function showEmptyFields() {
    $this->hide_empty_field = FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function getEmptyFieldDisplay() {
    return $this->hide_empty_field;
  }

  /**
   * {@inheritdoc}
   */
  public function enableAJAXLoading() {
    $this->ajax_field = TRUE;
  }

  /**
   * {@inheritdoc}
   */
  public function disableAJAXLoading() {
    $this->ajax_field = FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function getAJAXLoadingStatus() {
    return $this->hide_empty_field;
  }

  // --------------------------------------------------------------------------
  //                             TYPE SORTING
  //
  // The following methods pertain to sorting Tripal Entity Types for listing.
  // --------------------------------------------------------------------------

  /**
   * Sorts Tripal Entity Types first by category and then by Label.
   *
   * @param $a
   *   The first Tripal Entity Type object.
   * @param $b
   *   The second Tripal Entity Type object.
   */
  public static function sortByCategory(TripalEntityTypeInterface $a, TripalEntityTypeInterface $b) {
    $a_value = $a->getCategory();
    $b_value = $b->getCategory();
    if ($a_value == $b_value) {
      $a_label = $a
        ->label() ?? '';
      $b_label = $b
        ->label() ?? '';
      return strnatcasecmp($a_label, $b_label);
    }
    if ($a_value == 'General') {
      return -1;
    }
    elseif ($b_value == 'General') {
      return 1;
    }
    else {
      return strnatcasecmp($b_value, $a_value);
    }
  }

}
