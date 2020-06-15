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
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $tripal_entity_type->label(),
      '#description' => $this->t("Label for the Tripal Content type."),
      '#required' => TRUE,
    ];


    // Determine the machine name for the content type.
    if ($tripal_entity_type->isNew()) {

      $db = \Drupal::database();
      $highest_name = $db->select('config', 'c')
        ->fields('c', ['name'])
        ->condition('c.name', 'tripal.bio_data.', '~')
        ->orderBy('c.name', 'DESC')
        ->execute()
        ->fetchField();
      if ($highest_name) {
        $max_index = str_replace(
          'tripal.bio_data.bio_data_',
          '',
          $highest_name
        );
        $default_id = $max_index + 1;
      }
      else {
        $default_id = 1;
      }
    }
    else {
      $default_id = $tripal_entity_type->getID();
    }
    $form['name'] = [
      '#type' => 'machine_name',
      '#default_value' => 'bio_data_' . $default_id,
      '#required' => TRUE,
      '#machine_name' => [
        'exists' => '\Drupal\tripal\Entity\TripalEntityType::load',
      ],
      '#disabled' => TRUE,
    ];
    $form['id'] = [
      '#type' => 'hidden',
      '#value' => $default_id,
    ];

    // We need to choose a term if this is a new content type.
    // The term cannot be changed later!
    if ($tripal_entity_type->isNew()) {
      $description = t('The Tripal controlled vocabulary term (cv) term which characterizes this content type. For example, to create a content type for storing "genes", use the "gene" term from the Sequence Ontology (SO). <strong>The Tripal CV Term must already exist; you can <a href="@termUrl">add a Tripal CV Term here</a>.</strong>',
        ['@termUrl' => Url::fromRoute('entity.tripal_vocab.collection')->toString()]);
      $form['term_id'] = [
        '#type' => 'entity_autocomplete',
        '#title' => 'Tripal Controlled Vocabulary (CV) Term',
        '#description' => $description,
        '#target_type' => 'tripal_term',
        '#required' => TRUE,
      ];
    }
    else {
      $term = $tripal_entity_type->getTerm();
      $vocab = $term->getVocab();
      // Save the term for later.
      $form['term_id'] = [
        '#type' => 'hidden',
        '#value' => $term->getId(),
      ];
      // Describe the term to the user but do not allow them to change it.
      $form['term'] = [
        '#type' => 'table',
        '#caption' => 'Controlled Vocabulary Term',
        '#rows' => [
          [
            ['header' => TRUE, 'data' => 'Vocabulary'],
            $vocab->getLabel()
          ],
          [
            ['header' => TRUE, 'data' => 'Name'],
            $term->getName()
          ],
          [
            ['header' => TRUE, 'data' => 'Accession'],
            $term->getAccession()
          ],
        ],
        '#weight' => -8,
      ];
    }

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
<p>hTe pattern below is used to specify the URL of content pages of this type. This allows you to present more friendly, informative URLs to your user.</p>

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

    // Ensure the cvterm has not already been used for another Content Type.
    if ($tripal_entity_type->isNew()) {
      $entities = \Drupal::entityTypeManager()
        ->getStorage('tripal_entity_type')
        ->loadByProperties(['term_id' => $values['term_id']]);
      if (!empty($entities)) {
        $form_state->setErrorByName('term_id',
        $this->t('A Tripal Content type with choosen Tripal Controlled Vocabulay Term already exists. Please choose a unique term.'));
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $tripal_entity_type = $this->entity;

    // Set the basic values of a Tripal Entity Type.
    $tripal_entity_type->setID($values['id']);
    $tripal_entity_type->setName($values['name']);
    $tripal_entity_type->setLabel($values['label']);
    $tripal_entity_type->setHelpText($values['help']);
    $tripal_entity_type->setTerm($values['term_id']);
    $tripal_entity_type->setTitleFormat($values['title_format']);
    $tripal_entity_type->setURLFormat($values['url_format']);

    // Finally, save the entity we've compiled.
    $status = $tripal_entity_type->save();

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
