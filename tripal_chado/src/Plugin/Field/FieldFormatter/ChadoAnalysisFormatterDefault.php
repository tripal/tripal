<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal analysis formatter.
 *
 * @FieldFormatter(
 *   id = "chado_analysis_formatter_default",
 *   label = @Translation("Chado analysis formatter"),
 *   description = @Translation("A chado analysis formatter"),
 *   field_types = {
 *     "chado_analysis_default"
 *   },
 *   valid_tokens = {
 *     "[name]",
 *     "[description]",
 *     "[program]",
 *     "[programversion]",
 *     "[algorithm]",
 *     "[sourcename]",
 *     "[sourceversion]",
 *     "[sourceuri]",
 *   },
 * )
 */
class ChadoAnalysisFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['token_string'] = '[name]';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $list = [];
    $token_string = $this->getSetting('token_string');

    foreach($items as $delta => $item) {
      $values = [
        'name' => $item->get('analysis_name')->getString(),
        'description' => $item->get('analysis_description')->getString(),
        'program' => $item->get('analysis_program')->getString(),
        'programversion' => $item->get('analysis_programversion')->getString(),
        'algorithm' => $item->get('analysis_algorithm')->getString(),
        'sourcename' => $item->get('analysis_sourcename')->getString(),
        'sourceversion' => $item->get('analysis_sourceversion')->getString(),
        'sourceuri' => $item->get('analysis_sourceuri')->getString(),
        // timeexecuted not implemented
      ];

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }
      $list[$delta] = $displayed_string;
    }

    // Only return markup if the field is not empty.
    if (!empty($list)) {

      // If only one element has been found, don't make into a list.
      if (count($list) == 1) {
        $elements[0] = [
          "#markup" => $list[0],
        ];
      }

      // If more than one value has been found, display all values in an
      // unordered list.
// @todo: add a pager
      elseif (count($list) > 1) {
        $elements[0] = [
          '#theme' => 'item_list',
          '#list_type' => 'ul',
          '#items' => $list,
          '#wrapper_attributes' => ['class' => 'container'],
        ];
      }
    }

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

    $form['token_string'] = [
      '#title' => $this->t('Token string for field display'),
      '#description' => $this->t('You may specify elements in this text box to customize how'
                     . ' contacts are displayed. The available tokens are [name] for the'
                     . ' analysis name, [program], [programversion], [algorithm],'
                     . ' [sourcename], [sourceversion], [sourceuri]'
                     . ' For example, "[name] ([sourcename])"'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('token_string'),
      '#element_validate' => [[static::class, 'settingsFormValidateTokenString']],
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();
    $summary[] = $this->t('Set display format');
    return $summary;
  }

}
