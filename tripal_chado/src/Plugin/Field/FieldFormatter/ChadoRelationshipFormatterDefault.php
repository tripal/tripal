<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;
use Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoCvtermBuddy;

/**
 * Plugin implementation of default Tripal Relationship formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_relationship_formatter_default',
  label: new TranslatableMarkup('Chado Relationship Formatter'),
  description: new TranslatableMarkup('A chado relationship formatter'),
  field_types: [
    'chado_relationship_type_default',
    'chado_relationship_by_role_type_default',
  ],
  valid_tokens: [
    '[accession]',
    '[version]',
    '[description]',
    '[db_name]',
    '[db_description]',
    '[db_urlprefix]',
    '[db_url]',
  ],
)]
class ChadoRelationshipFormatterDefault extends ChadoFormatterBase {

  /**
   * Used to store the cvterm ChadoBuddy instance.
   *
   * @var \Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoCvtermBuddy
   */
  protected ChadoCvtermBuddy $cvterm_instance;

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['token_string'] = 'The [subject_bundle] <strong>[subject_name]</strong> [type_name] [object_bundle] <strong>[object_name]</strong>';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $list = [];
    $token_string = $this->getSetting('token_string');
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');

    foreach ($items as $delta => $item) {
      $values = [
        'record_id' => $item->get('record_id')->getString(),
        'subject_id' => $item->get('subject_id')->getString(),
        'subject_name' => $item->get('subject_name')->getString(),
        'subject_entity_id' => $item->get('subject_entity_id')->getString(),
        'object_id' => $item->get('object_id')->getString(),
        'object_name' => $item->get('object_name')->getString(),
        'object_entity_id' => $item->get('object_entity_id')->getString(),
        'type_id' => $item->get('type_id')->getString(),
        'type_name' => $item->get('type_name')->getString(),
      ];
      $direction = 1;
      if ($values['subject_id'] == $values['record_id']) {
        $direction = -1;
      }

      // If this is a "by role" field the type_name may not be stored
      // on the item. Attempt to resolve it from the type_id or from the
      // field settings so the formatter can display a meaningful name.
      if (empty($values['type_name'])) {
        $field_definition = $items->getFieldDefinition();
        if ($field_definition->getType() === 'chado_relationship_by_role_type_default') {
          $type_id = (int) $item->get('type_id')->getString();
          if ($type_id) {
            $values['type_name'] = $this->cvterm_instance->getCvterm([
              'cvterm_id' => $type_id,
            ])->getName();
          }
          else {
            // As a fallback, try to read term info from field settings.
            $settings = $field_definition->getSettings();
            if (!empty($settings['termIdSpace']) && !empty($settings['termAccession'])) {
              $idspace = $settings['termIdSpace'];
              $accession = $settings['termAccession'];
              $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
              $idSpace = $idSpace_manager->loadCollection($idspace);
              $term = $idSpace ? $idSpace->getTerm($accession) : NULL;
              if ($term) {
                $values['type_name'] = $term->getName();
              }
            }
          }
        }
      }

      // As we did in Tripal 3, the term is processed up a bit to make for nicer display.
      $this->formatTypeName($values);

      // Create a clickable link to the corresponding related entity when one exists.
      // We need to pre-render so that we can replace this into the token string.
      if ($direction == 1) {
        $item = $lookup_manager->getRenderableItem($values['subject_name'], $values['subject_entity_id']);
        $values['subject_name'] = \Drupal::service('renderer')->render($item);
      }
      else {
        $item = $lookup_manager->getRenderableItem($values['object_name'], $values['object_entity_id']);
        $values['object_name'] = \Drupal::service('renderer')->render($item);
      }

      // Lookup subject and object entity bundle names. ID may be -1 if unpublished.
      // e.g. for features: mRNA xxx is part of Gene yyy.
      $values['subject_bundle'] = $lookup_manager->getBundleLabel($values['subject_entity_id']);
      $values['object_bundle'] = $lookup_manager->getBundleLabel($values['object_entity_id']);

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }

      $list[$delta] = [
        '#markup' => $displayed_string,
      ];
    }

    // If only one element has been found, don't make into a list.
    if (count($list) == 1) {
      $elements = $list;
    }

    // If more than one value has been found, display all values in an
    // unordered list.
    elseif (count($list) > 1) {
      $elements[0] = [
        '#theme' => 'item_list',
        '#list_type' => 'ul',
        '#items' => $list,
        '#wrapper_attributes' => ['class' => 'container'],
      ];
    }

    return $elements;
  }

  /**
   * Format the controlled vocabulary term for nicer display.
   * Tripal 3: tripal_chado/includes/TripalFields/sbo__relationship/sbo__relationship_formatter.inc
   *
   * @param array &$values
   *   The associative array of field values.
   *   The value with the key 'term_name' is updated.
   */
  protected function formatTypeName(&$values): void {
    $verb = $this->get_rel_verb($values['type_name']);
    $values['type_name'] = $verb . preg_replace('/_/', ' ', $values['type_name']);
  }

  /**
   * A helper function to define English verbs for relationship types.
   *
   * @param string $rel_type
   *   The vocabulary term name for the relationship.
   *
   * @return string
   *   The verb to use when creating a sentence of the relationship.
   */
  protected function get_rel_verb(string $rel_type): string {
    $rel_type_clean = lcfirst(preg_replace('/_/', ' ', $rel_type));
    $verb = '';
    // Can skip anything already starting with 'is' or 'has'.
    if (!preg_match('/^(is|has) /', $rel_type_clean)) {
      switch ($rel_type_clean) {
        case 'integral part of':
        case 'instance of':
          $verb = 'is an';
          break;

        case 'genome of':
        case 'part of':
        case 'position of':
        case 'proper part of':
        case 'sequence of':
        case 'transformation of':
        case 'variant of':
          $verb = 'is a';
          break;

        case 'connects on':
        case 'contains':
        case 'derives from':
        case 'finishes':
        case 'guides':
        case 'maximally overlaps':
        case 'overlaps':
        case 'starts':
          break;

        default:
          $verb = 'is';
      }
    }
    // Since verb may be an empty string, only space pad when something is present.
    if ($verb) {
      $verb .= ' ';
    }
    return $verb;
  }

}
