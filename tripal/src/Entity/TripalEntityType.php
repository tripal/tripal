<?php

namespace Drupal\tripal\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;

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
 *       "html" = "Drupal\tripal\TripalEntityTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "bio_data",
 *   admin_permission = "administer tripal content types",
 *   bundle_of = "tripal_entity",
 *   entity_keys = {
 *     "id" = "name",
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
 *     "name",
 *     "label",
 *     "term_id",
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
   * The Tripal Content machine name.
   *
   * @var string
   */
  protected $name;

  /**
   * The Tripal Content type label.
   *
   * @var string
   */
  protected $label;

  /**
   * The Tripal Term which describes this content type.
   *
   * @var integer
   */
  protected $term_id;

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
  //                          MAIN SETTER / GETTERS
  //
  // The following methods allow the main properties of the Tripal Entity Type
  // to be set or retrieved. These properties include ID, macine name, term
  // help text and category.
  // --------------------------------------------------------------------------

  /**
   * {@inheritdoc}
   */
  public function id() {
    return $this->name;
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
  public function setID($id) {
    $this->id = $id;
  }

  /**
   * {@inheritdoc}
   */
  public function getName() {
    return $this->name;
  }

  /**
   * {@inheritdoc}
   */
  public function setName($name) {
    $this->name = $name;
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
  public function getTerm() {
    $term = \Drupal\tripal\Entity\TripalTerm::load($this->term_id);
    return $term;
  }

  /**
   * {@inheritdoc}
   */
  public function setTerm($term_id) {
    $this->term_id = $term_id;
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
      return $format;
    }

    // B) Generate our own ugly title by simply comma-separating all the
    //    required fields.
    if (!$format) {
      $tmp = [];

      // Check which tokens are required fields and join them into a default
      // format.
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
      return $format;
    }

    return $format;
  }

  // --------------------------------------------------------------------------
  //                             URL ALIAS FORMATS
  //
  // The following methods all pertain to setting alias' for Tripal Content
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
      return '[type]/[TripalEntity__entity_id]';
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
      $token = '[TripalBundle__bundle_id]';
      $tokens[$token] = [
        'label' => 'Bundle ID',
        'description' => 'The unique identifier for this Tripal Content Type.',
        'token' => $token,
        'field_name' => NULL,
        'required' => TRUE,
      ];

      $token = '[TripalEntity__entity_id]';
      $tokens[$token] = [
        'label' => 'Content/Entity ID',
        'description' => 'The unique identifier for an individual piece of Tripal Content.',
        'token' => $token,
        'field_name' => NULL,
        'required' => TRUE,
      ];
    }

    // Term/Type Tokens:
    $token = '[TripalEntityType__label]';
    $tokens[$token] = [
      'label' => 'Tripal Entity Type',
      'description' => 'The human-readable label for this Tripal Content Type.',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE,
    ];

    $token = '[TripalTerm__vocab]';
    $tokens[$token] = [
      'label' => 'Tripal Vocab Short Name',
      'description' => 'The short vocabulary name for the Tripal Term desribing this Tripal Content Type.',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE,
    ];

    $token = '[TripalTerm__name]';
    $tokens[$token] = [
      'label' => 'Tripal Term Label',
      'description' => 'The human-readable name for the Tripal Term desribing this Tripal Content Type.',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE,
    ];

    $token = '[TripalTerm__accession]';
    $tokens[$token] = [
      'label' => 'Tripal Term Accession',
      'description' => 'The unique accession for the Tripal Term desribing this Tripal Content Type.',
      'token' => $token,
      'field_name' => NULL,
      'required' => TRUE,
    ];

    $instances = \Drupal::service('entity_field.manager')
      ->getFieldDefinitions('tripal_entity', $this->name);
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
}
