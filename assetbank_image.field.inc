<?php

/**
 * @file
 * Implement an assetbank_image field, based on the file module's file field.
 */

/**
 * Implements hook_field_info().
 */
function assetbank_image_field_info() {
  return array(
    'assetbank_image' => array(
      'label' => t('AssetBank Image'),
      'description' => t('This field stores the Asset ID of an image as an integer value.'),
      'default_widget' => 'assetbank_image_text',
      'default_formatter' => 'assetbank_image',
    ),
  );
}

/**
 * Implements hook_field_is_empty().
 */
function assetbank_image_field_is_empty($item, $field) {
  return empty($item['assetbank_asset_id']);
}

/**
 * Implements hook_field_widget_info().
 */
function assetbank_image_field_widget_info() {
  return array(
    'assetbank_image_text' => array(
      'label' => t('Manual Asset ID'),
      'field types' => array('assetbank_image'),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function assetbank_image_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $instance_options = assetbank_instance_options();

  // Disable fields if there are no configured instances.
  $disabled = FALSE;

  if (count($instance_options) < 1) {
    $disabled = TRUE;
  }

  $widget['#delta'] = $delta;

  switch ($instance['widget']['type']) {
    case 'assetbank_image_text':
      $widget['assetbank_instance_name'] = array(
        '#title' => t('AssetBank'),
        '#type' => 'select',
        '#default_value' => isset($items[$delta]['assetbank_instance_name']) ? $items[$delta]['assetbank_instance_name'] : '',
        '#options' => $instance_options,
        '#description' => t('Select the AssetBank installation where this image resides. If this field is disabled, there are no AssetBank installation configured. Configure them !assetbank_config.', array('!assetbank_config' => l(t('here'), 'admin/config/media/assetbank'))),
        '#required' => $element['#required'],
        '#disabled' => $disabled,
      );

      $widget['assetbank_asset_id'] = array(
        '#title' => t('AssetBank Asset ID'),
        '#type' => 'textfield',
        '#default_value' => isset($items[$delta]['assetbank_asset_id']) ? $items[$delta]['assetbank_asset_id'] : '',
        '#description' => t('The numeric Asset ID of the image you wish to use.'),
        '#size' => 10,
        '#maxlength' => 10,
        '#required' => $element['#required'],
        '#disabled' => $disabled,
      );

      // Add the additional alt and title fields.
      $widget['alt'] = array(
        '#title' => t('Alternate text'),
        '#type' => 'textfield',
        '#default_value' => isset($items[$delta]['alt']) ? $items[$delta]['alt'] : '',
        '#description' => t('This text will be used by screen readers, search engines, or when the image cannot be loaded.'),
        '#maxlength' => 512,
        '#weight' => 2,
        '#disabled' => $disabled,
      );

      $widget['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#default_value' => isset($items[$delta]['title']) ? $items[$delta]['title'] : '',
        '#description' => t('The title is used as a tool tip when the user hovers the mouse over the image.'),
        '#maxlength' => 1024,
        '#weight' => 1,
        '#disabled' => $disabled,
      );
      break;
  }

  $element += $widget;
  return $element;
}

/**
 * Implements hook_field_validate().
 */
function assetbank_image_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  foreach ($items as $delta => $item) {
    if (!empty($item['assetbank_asset_id'])) {
      if (!is_numeric(($item['assetbank_asset_id']))) {
        $errors[$field['field_name']][$langcode][$delta][] = array(
          'error' => 'assetbank_image_invalid',
          'message' => t('Asset ID must be numeric.'),
        );
      }
    }
  }
}

/**
 * Implements hook_field_formatter_info().
 */
function assetbank_image_field_formatter_info() {
  $formatters = array(
    'assetbank_image' => array(
      'label' => t('AssetBank Image'),
      'field types' => array('assetbank_image'),
      'settings' => array('assetbank_image_style' => 'default'),
    ),
  );

  return $formatters;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function assetbank_image_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {

  $assetbank_image_style_options = assetbank_image_style_options(FALSE, PASS_THROUGH);

  $element['assetbank_image_style'] = array(
    '#title' => t('Image style'),
    '#type' => 'select',
    '#default_value' => $display['settings']['assetbank_image_style'],
    '#options' => $assetbank_image_style_options,
  );

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function assetbank_image_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = array();

  $assetbank_image_styles = assetbank_image_style_options(FALSE, PASS_THROUGH);
  $summary[] = t('Image style: @style', array('@style' => $assetbank_image_styles[$settings['assetbank_image_style']]));

  return implode('<br />', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function assetbank_image_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  foreach ($items as $delta => $item) {
    $element[$delta] = array(
      '#theme' => 'assetbank_image_formatter',
      '#item' => $item,
      '#assetbank_image_style' => $display['settings']['assetbank_image_style'],
    );
  }

  return $element;
}

/**
 * Returns HTML for an image field formatter.
 *
 * @param array $variables
 *   An associative array containing:
 *   - item: Associative array of image data, which may include "alt",
 *     "width", "height", "title" and "attributes".
 *   - assetbank_image_style: An AssetBank Image Style.
 *   - assetbank_instance_name: An AssetBank Instance Name.
 *
 * @ingroup themeable
 */
function theme_assetbank_image_formatter($variables) {
  $item = $variables['item'];

  if (array_key_exists('alt', $item)) {
    $image['alt'] = $item['alt'];
  }

  if (isset($item['attributes'])) {
    $image['attributes'] = $item['attributes'];
  }

  if (isset($item['width']) && isset($item['height'])) {
    $image['width'] = $item['width'];
    $image['height'] = $item['height'];
  }

  // Do not output an empty 'title' attribute.
  if (isset($item['title']) && drupal_strlen($item['title']) > 0) {
    $image['title'] = $item['title'];
  }
  $image['asset_id'] = $item['assetbank_asset_id'];
  $image['assetbank_name'] = $item['assetbank_instance_name'];
  $image['style_name'] = $variables['assetbank_image_style'];
  $output = theme('assetbank_image_style', $image);

  return $output;
}
