<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerInterface;
use Drupal\Core\Url;

/**
 * Class TripalEntityTypeForm.
 *
 * @package Drupal\tripal\Form
 */
class TripalEntityTypeForm extends EntityForm {

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    $tripal_entity_type = $this->entity;
    $tripal_entity_type->setDefaults();

    // Determine the machine name for the content type.
    // Note, there may be some discrepancy here if others happen to be creating
    // content types at the same time. Here we are making a prediction but
    // the TripalEntityType->save() method will create the id at the time of
    // saving, not based on what we provide it here.
    if ($tripal_entity_type->isNew()) {
      $config = \Drupal::config('tripal.settings');
      $max_index = $config->get('tripal_entity_type.max_id');
      $machine_name = 'bio_data_' . ($max_index + 1);
    }
    else {
      $machine_name = $tripal_entity_type->id();
    }

    // We need to choose a term if this is a new content type.
    // The term cannot be changed later!
    $term_autocomplete_default = '';
    $disabled = NULL;
    if ($tripal_entity_type->isNew()) {
      $term_autocomplete_default = $form_state->getValue('term');
      $disabled = FALSE;
    }
    // As mentioned above, the term cannot be changed later!
    // As such, if this is not a new content type then we will only take into
    // account the set term (not the form state) and disable the field.
    // NOTE: we go this route because only showing the field on the add page
    // causes a validation error.
    else {
      $vocab_label = $term_name = $term_accession = '';

      $term = $tripal_entity_type->getTerm();
      if ($term) {
        $vocab = $term->getVocabularyObject();

        $vocab_label = $vocab->getLabel();
        $term_name = $term->getName();
        $term_accession = $term->getTermId();

        $term_autocomplete_default = $term->getName() . ' (' . $term->getTermId() . ')';
        $disabled = TRUE;
      }

      // We also want to add an element at the top that fully describes the term.
      // So lets do that here and use weight to put it at the top.
      $form['term_description'] = [
        '#type' => 'table',
        '#caption' => 'Controlled Vocabulary Term',
        '#rows' => [
          [
            'Vocabulary',
            $vocab_label
          ],
          [
            'Name',
            $term_name
          ],
          [
            'Accession',
            $term_accession
          ],
        ],
        '#weight' => -8,
      ];
    }

    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $tripal_entity_type->label(),
      '#description' => $this->t("Label for the Tripal Content type."),
      '#required' => TRUE,
    ];

    $form['name'] = [
      '#type' => 'machine_name',
      '#default_value' => $machine_name,
      '#required' => TRUE,
      '#machine_name' => [
        'exists' => '\Drupal\tripal\Entity\TripalEntityType::load',
      ],
      '#disabled' => TRUE,
    ];

    $form['term'] = [
      '#type' => 'textfield',
      '#title' => 'Controlled Vocabulary Term',
      '#required' => TRUE,
      '#description' => $this->t('Enter a vocabulary term name. A set of matching ' .
        'candidates will be provided to choose from. You may find the multiple matching terms ' .
        'from different vocabularies. The full accession for each term is provided ' .
        'to help choose. Only the top 10 best matches are shown at a time.'),
      '#default_value' => $term_autocomplete_default,
      '#disabled' => $disabled,
      '#autocomplete_route_name' => 'tripal.cvterm_autocomplete',
      '#autocomplete_route_parameters' => array('count' => 10),
    ];

    $description = "A grouping category for this Tripal Content type. It should be the same as other Tripal Content types and can be used to group similar biological data types to make them easier to find.";
    $form['category'] = [
      '#type' => 'textfield',
      '#title' => 'Category',
      '#description' => $description,
      '#default_value' => $tripal_entity_type->getCategory(),
      '#required' => TRUE,
    ];

    // Allow the administrator to set help text for users.
    $form['help'] = [
      '#type' => 'textarea',
      '#title' => 'Help Text',
      '#description' => 'This is shown to administrators to further explain this Tripal content type. For example, this can be used to provide an example or site-specific instructions.',
      '#default_value' => $tripal_entity_type->getHelpText(),
      '#required' => TRUE,
    ];

    // ADVANCED SETTINGS:
    $form['advanced'] = array(
      '#type' => 'vertical_tabs',
      '#title' => t('Advanced Settings'),
    );

    $tokens = $tripal_entity_type->getTokens();
    $title_tokens = $tokens;
    unset($title_tokens['[title]']);
    unset($title_tokens['[TripalBundle__bundle_id]']);
    unset($title_tokens['[TripalEntity__entity_id]']);

    // Page title options:
    $form['title_settings'] = [
      '#type' => 'details',
      '#title' => 'Page title options',
      '#group' => 'advanced',
    ];

    $form['title_settings']['msg'] = [
      '#type' => 'item',
      '#markup' => t('
<p>The format below is used to determine the title displayed on content pages of the current type. This ensures all content of this type is consistent while still allowing you to indicate which data you want represented in the title (ie: which data would most identify your content).</p>

<p>Keep in mind that it might be confusing to users if more than one page has the same title. <strong>We recommend you choose a combination of tokens that will uniquely identify your content</strong>.</p>'),
    ];

    $form['title_settings']['title_format'] = [
      '#type' => 'textfield',
      '#title' => 'Page Title Format',
      '#description' => 'You may rearrange elements in this text box to customize the page titles. The available tokens are listed below. You can separate or include any text between the tokens.',
      '#default_value' => $tripal_entity_type->getTitleFormat(),
    ];

    $form['title_settings']['tokens'] = [
      '#type' => 'fieldset',
      '#title' => 'Available Tokens',
    ];

    $form['title_settings']['tokens']['msg'] = [
      '#type' => 'markup',
      '#markup' => 'Copy the token and paste it into the "Page Title Format" text field above.'
    ];

    $form['title_settings']['tokens']['content'] =
      theme_token_list($title_tokens);

    // URL Alias options:
    $form['url_settings'] = [
      '#type' => 'details',
      '#title' => 'URL alias options',
      '#group' => 'advanced',
    ];

    $form['url_settings']['msg'] = [
      '#type' => 'item',
      '#markup' => t('
<p>The pattern below is used to specify the URL of content pages of this type. This allows you to present more friendly, informative URLs to your user.</p>

<p><strong>You must choose a combination of tokens that results in a unique path for each page!</strong></p>'),
    ];

    $form['url_settings']['url_format'] = [
      '#type' => 'textfield',
      '#title' => 'URL Alias Pattern',
      '#description' => 'You may rearrange elements in this text box to customize the url alias. The available tokens are listed below. <strong>Make sure the pattern forms a valid, unique URL.</strong> Leave this field blank to use the original path.',
      '#default_value' => $tripal_entity_type->getURLFormat(),
    ];

    $form['url_settings']['tokens'] = [
      '#type' => 'fieldset',
      '#title' => 'Available Tokens',
    ];

    $form['url_settings']['tokens']['msg'] = [
      '#type' => 'markup',
      '#markup' => 'Copy the token and paste it into the "URL Alias Pattern" text field above.'
    ];

    $form['url_settings']['tokens']['content'] = theme_token_list($tokens);

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    parent::validateForm($form, $form_state);

    $values = $form_state->getValues();
    $tripal_entity_type = $this->entity;

    // Ensure the label is not already taken.
    $entities = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties(['label' => $values['label']]);
    unset($entities[ $values['name'] ]);
    if (!empty($entities)) {
      $form_state->setErrorByName('label',
        $this->t('A Tripal Content type with the label :label already exists. Please choose a unique label.', [':label' => $values['label']]));
    }

    $term_str = $form_state->getValue('term');
    $matches = [];
    if (preg_match('/(.+?) \((.+?):(.+?)\)/', $term_str, $matches)) {
      $idSpace = $matches[2];
      $accession = $matches[3];

      // Ensure the term has not already been used for another Content Type.
      if ($tripal_entity_type->isNew()) {
        $entity_query = \Drupal::entityTypeManager()
          ->getStorage('tripal_entity_type')
          ->getQuery();
        // We don't want to restrict by permission here b/c we want to ensure
        // that no type has this term whether the user has permission to see
        // it or not.
        $entity_query
          ->accessCheck(FALSE)
          ->condition('termIdSpace', $idSpace)
          ->condition('termAccession', $accession);
        $entities = $entity_query->execute();

        if (!empty($entities)) {
          $form_state->setErrorByName('term',
          $this->t('A Tripal Content Type with this controlled vocabulay term already exists. Please choose a unique term.'));
        }
      }

      // Ensure the term exists.
      // We look up the term using the ID Space plugin manager.
      $idSpace_object = \Drupal::service('tripal.collection_plugin_manager.idspace')
        ->loadCollection($idSpace);
      if ($idSpace_object === NULL) {
        $form_state->setErrorByName('term',
          $this->t('You entered "%termStr" but the ID Space, "%idspace", does not exist. Please select an existing term from the autocomplete drop-down.',
          ['%termStr' => $term_str, '%idspace' => $idSpace]
        ));
      }
      else {
      $term_object = $idSpace_object->getTerm($accession);
        if ($term_object === NULL) {
          $form_state->setErrorByName('term',
            $this->t('You entered "%termStr" but a term with the accession, "%accession", does not exist in that ID Space. Please select an existing term from the autocomplete drop-down.',
            ['%termStr' => $term_str, '%accession' => $accession]
          ));
        }
      }
    }
    else {
      $form_state->setErrorByName('term',
          'Please select a term from the autocomplete drop-down. It must have the ID space and accession in parenthesis.');
    }
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {

    // Grab the entity associated with this form for easier access to create/update it.
    $tripal_entity_type = $this->entity;
    // Get all the values from the form state at once for greater readability below.
    $values = $form_state->getValues();

    // Note: we have to reprocess the term ID Space + Accession because we can't
    // save what we learned in validate. We do not use an if around the preg_replace()
    // because in order to get here, this pattern has to work.
    preg_match('/(.+?) \((.+?):(.+?)\)/', $values['term'], $matches);
    $idSpace = $matches[2];
    $accession = $matches[3];

    // Set the properties for the new Tripal Content Type
    // using those set in the form state.
    $tripal_entity_type->setLabel($values['label']);
    $tripal_entity_type->setHelpText($values['help']);
    $tripal_entity_type->setTermIdSpace($idSpace);
    $tripal_entity_type->setTermAccession($accession);
    $tripal_entity_type->setTitleFormat($values['title_format']);
    $tripal_entity_type->setURLFormat($values['url_format']);

    // Finally, save the entity we've compiled.
    $status = $tripal_entity_type->save();

    // and let the use know if we were successful.
    $messenger = $this->messenger();
    switch ($status) {
      case SAVED_NEW:
        $messenger->addMessage($this->t('Created the %label Tripal Content Type.', [
          '%label' => $tripal_entity_type->label(),
        ]));
        break;

      default:
        $messenger->addMessage($this->t('Saved changes %label Tripal Content Type.', [
          '%label' => $tripal_entity_type->label(),
        ]));
    }
    $form_state->setRedirectUrl($tripal_entity_type->toUrl('collection'));
  }

}
