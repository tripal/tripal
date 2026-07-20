<?php

namespace Drupal\tripal_search_views\Plugin\views\filter;

use Drupal\Core\Form\FormStateInterface;
use Drupal\views\Plugin\views\filter\StringFilter;
use Drupal\views\Attribute\ViewsFilter;

/**
 * String filter with dynamic select options.
 *
 * @ingroup views_filter_handlers.
 */
#[ViewsFilter("dynamic_filter")]
class InformedStringFilter extends StringFilter {

  /**
   * Indicates a single filter for an exposed view filter.
   */
  const TYPE_SINGLE_FILTER = 0;

  /**
   * Indicates a grouped filter for an exposed view filter.
   */
  const TYPE_GROUPED_FILTER = 1;

  /**
   * Indicates a dynamic select list for an exposed view filter.
   */
  const TYPE_DYNAMIC_SELECT_LIST = 2;

  /**
   * {@inheritDoc}
   */
  public function defineOptions() {
    $options = parent::defineOptions();
    $options['filter_type'] = ['default' => self::TYPE_SINGLE_FILTER];
    return $options;
  }

  /**
   * {@inheritDoc}
   */
  public function buildExposedForm(&$form, FormStateInterface $form_state) {
    $form_state->set('exposed', $this->options['exposed']);
    parent::buildExposedForm($form, $form_state);

    $view = $this->view;
    $type = $this->options['filter_type'] ?? self::TYPE_SINGLE_FILTER;
    // Only modify the exposed form if this is a filter for a field on
    // Tripal Content and the field name exists.
    if (!$view || $view->storage->get('base_table') !== 'tripal_entity' || !isset($this->definition['field_name']) || $type != self::TYPE_DYNAMIC_SELECT_LIST) {
      return;
    }

    // Get the identifier for this filter.
    $identifier = $this->options['expose']['identifier'];

    // Get the field name from the table name.
    $field_name = $this->definition['field_name'];

    // Get the field value property from the definition.
    $real_field = $this->realField;

    // The property obtained from the real field.
    $property = str_replace($field_name . '_', '', $real_field);

    // Get the unique values for this field and build options for the select.
    $results = \Drupal::service('tripal.fieldvalue.lookup')->getUniqueFieldValues($field_name, $property, [], []);
    $options = [];
    foreach ($results as $row) {
      $value = $row[$real_field];
      $options[$value] = $value;
    }
    natcasesort($options);
    // Replace the default text input with a select element.
    $form[$identifier] = [
      '#type' => 'select',
      '#title' => $this->options['expose']['label'],
      '#empty_option' => $this->t('- Any -'),
      '#empty_value' => '',
      '#options' => $options,
      '#default_value' => '',
    ];
  }

  /**
   * {@inheritDoc}
   */
  protected function showBuildGroupButton(&$form, FormStateInterface $form_state) {
    $form['group_button'] = [
      '#prefix' => '<div class="views-grouped clearfix">',
      '#suffix' => '</div>',
      // Should always come after the description and the relationship.
      '#weight' => -190,
    ];
    $grouped_description = $this->t('Grouped filters allow a choice between predefined operator|value pairs.');
    $form['group_button']['radios'] = [
      '#theme_wrappers' => ['container'],
      '#attributes' => ['class' => ['js-only']],
    ];
    $form['group_button']['radios']['radios'] = [
      '#title' => $this->t('Filter type to expose'),
      '#description' => $grouped_description,
      '#type' => 'radios',
      '#options' => [
        self::TYPE_SINGLE_FILTER => $this->t('Single filter'),
        self::TYPE_GROUPED_FILTER => $this->t('Grouped filters'),
        self::TYPE_DYNAMIC_SELECT_LIST => $this->t('Dynamic Select List'),
      ],
    ];
    $type = $this->options['filter_type'] ?? self::TYPE_SINGLE_FILTER;
    if ($type == self::TYPE_SINGLE_FILTER) {
      $form['group_button']['markup'] = [
        '#markup' => '<div class="description grouped-description">' . $grouped_description . '</div>',
      ];
      $form['group_button']['button'] = [
        '#limit_validation_errors' => [],
        '#type' => 'submit',
        '#value' => $this->t('Grouped filters'),
        '#submit' => [[$this, 'buildGroupForm']],
      ];
      $form['group_button']['radios']['radios']['#default_value'] = self::TYPE_SINGLE_FILTER;
    }
    elseif ($type == self::TYPE_GROUPED_FILTER) {
      $form['group_button']['button'] = [
        '#limit_validation_errors' => [],
        '#type' => 'submit',
        '#value' => $this->t('Single filter'),
        '#submit' => [[$this, 'buildGroupForm']],
      ];
      $form['group_button']['radios']['radios']['#default_value'] = self::TYPE_GROUPED_FILTER;
    }
    elseif ($type == self::TYPE_DYNAMIC_SELECT_LIST) {
      $form['group_button']['markup'] = [
        '#markup' => '<div class="description grouped-description">' .
        $this->t('Dynamic Select List allows selecting values dynamically.') .
        '</div>',
      ];
      $form['group_button']['button'] = [
        '#limit_validation_errors' => [],
        '#type' => 'submit',
        '#value' => $this->t('Dynamic Select List'),
        '#submit' => [[$this, 'buildGroupForm']],
      ];
      $form['group_button']['radios']['radios']['#default_value'] = self::TYPE_DYNAMIC_SELECT_LIST;
    }
  }

  /**
   * {@inheritDoc}
   */
  public function buildGroupForm($form, FormStateInterface $form_state) {
    $item = &$this->options;

    $input = $form_state->getUserInput();
    $selected = $input['options']['group_button']['radios']['radios'] ?? $item['filter_type']
    ?? $item['is_grouped'] ?? 0;

    $item['filter_type'] = $selected;
    $item['is_grouped'] = ($selected == 1);

    // Only build grouped options if needed.
    if (!empty($item['is_grouped'])) {
      $this->buildGroupOptions();
    }

    $view = $form_state->get('view');
    $display_id = $form_state->get('display_id');
    $type = $form_state->get('type');
    $id = $form_state->get('id');
    $view->getExecutable()->setHandler($display_id, $type, $id, $item);

    $view->addFormToStack($form_state->get('form_key'), $display_id, $type, $id, TRUE, TRUE);

    $view->cacheSet();
    $form_state->set('rerender', TRUE);
    $form_state->setRebuild();
    $form_state->get('force_build_group_options', TRUE);
  }

  /**
   * {@inheritDoc}
   */
  public function submitExposeForm($form, FormStateInterface $form_state) {
    $selected = $form_state->getValue(['options', 'group_button', 'radios', 'radios']);
    $this->options['filter_type'] = $selected;
    if ($selected == self::TYPE_DYNAMIC_SELECT_LIST) {
      $this->options['plugin_id'] = 'dynamic_filter';
    }
  }

}
