<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal\TripalField\TripalFormatterBase;

/**
 * Plugin implementation of the 'default_tripal_citation_type_formatter'.
 */
#[TripalFieldFormatter(
  id: 'default_tripal_citation_type_formatter',
  label: new TranslatableMarkup('Tripal Citation Formatter'),
  description: new TranslatableMarkup('The default citation type formatter.'),
  field_types: [
    'tripal_citation_type',
  ],
)]
class DefaultTripalCitationTypeFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();

    $settings['citation_template'] = '';

    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {

    $elements = [];

    $token_parser = \Drupal::service('tripal.token_parser');

    foreach ($items as $delta => $item) {
      $entity = $items->getEntity();
      $template = $this->getSetting('citation_template') ?? '';

      $selected_pub_id = $item->get('value')->getString();
      $publication = $this->getAssociatedPublication($selected_pub_id);
      if ($publication !== NULL) {
        $citation = $publication;
      }
      else {
        $values = $this->buildTokenValues($entity, $template);
        $citation = $token_parser->replaceTokens($template, $values);
        $citation = preg_replace('/\.\s*\./', '.', $citation);
        $citation = preg_replace('/,\s*,/', ',', $citation);
        $citation = preg_replace('/\s+/', ' ', $citation);
        $citation = trim($citation);
      }

      if ($citation !== '') {
        $elements[$delta] = [
          '#markup' => $citation,
        ];
      }
    }

    return $elements;
  }

  /**
   * Build token values keyed by entity field machine name.
   */
  protected function buildTokenValues($entity, string $template) {

    $values = [];

    preg_match_all('/\[([^\[\]]+)\]/', $template, $matches);
    $field_names = [];
    foreach ($matches[1] as $token_string) {
      foreach (explode('|', $token_string) as $field_name) {
        $field_names[$field_name] = TRUE;
      }
    }

    foreach (array_keys($field_names) as $field_name) {
      if (!$entity->hasField($field_name)) {
        continue;
      }

      $field = $entity->get($field_name);
      if ($field->isEmpty()) {
        continue;
      }

      $first_item = $field->first();
      $main_property = $first_item->mainDisplayPropertyName();

      $values[$field_name] = $first_item->getValue()[$main_property] ?? '';
    }

    return $values;
  }

  /**
   * Find an associated Chado publication and retrieve its citation.
   *
   * @return string|null
   *   The publication's citation, or NULL if no publication is associated.
   */
  protected function getAssociatedPublication(string $selected_pub_id): ?string {
    $id_space_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');

    $id_space = $id_space_manager->loadCollection('TPUB');
    $citation_term = $id_space->getTerm('0000003');

    $chado = \Drupal::service('tripal_chado.database');
    $citation = $chado->select('1:pubprop', 'pp')
      ->fields('pp', ['value'])
      ->condition('pub_id', (int) $selected_pub_id)
      ->condition('type_id', $citation_term->getInternalId())
      ->orderBy('rank')
      ->range(0, 1)
      ->execute()
      ->fetchField();

    if ($citation !== FALSE && $citation !== NULL) {
      return (string) $citation;
    }
    return NULL;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(
    array $form,
    FormStateInterface $form_state,
  ) {

    $form =
      parent::settingsForm(
        $form,
        $form_state
      );

    $form['citation_template'] = [
      '#type' => 'textarea',
      '#title' => $this->t('Generated citation template'),
      '#description' => $this->t('Used only when no associated publication is available. Enter entity field machine names in square brackets, separated by the punctuation you want in the citation, for example [field1].[field2].[field3].'),
      '#default_value' => $this->getSetting('citation_template'),
      '#rows' => 3,
      '#required' => FALSE,
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {

    $summary = parent::settingsSummary();

    $summary[] =
      $this->t(
        'Generated citation template: @template',
        [
          '@template' =>
            $this->getSetting('citation_template'),
        ]
      );

    return $summary;
  }

}
