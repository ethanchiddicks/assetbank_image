<?php

/**
 * @file
 * Administration pages for AssetBank configuration.
 */

/**
 * Menu callback listing of all current AssetBank Image Styles.
 */
function assetbank_image_style_list() {
  $page = array();

  $styles = assetbank_image_styles();
  $page['assetbank_image_style_list'] = array(
    '#markup' => theme('assetbank_image_style_list', array('styles' => $styles)),
  );

  return $page;
}

/**
 * Form builder; form for adding a new AssetBank Image Style.
 *
 * @ingroup forms
 */
function assetbank_image_style_form($form, &$form_state, $style = NULL) {
  if (isset($form_state['values'])) {
    $defaults = $form_state['values'];
  }
  elseif ($style) {
    $defaults = $style;
    $form_state['assetbank_image_style'] = $style;
  }
  else {
    $defaults = array(
      'label' => '',
      'name' => '',
      'width' => '100',
      'height' => '100',
      'preserve_aspect' => TRUE,
      'crop_to_fit' => FALSE,
      'fill_background' => FALSE,
      'fill_background_hexcode' => 'FFFFFF',
    );
  }

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Style name'),
    '#default_value' => $defaults['label'],
    '#disabled' => isset($style),
    '#required' => TRUE,
  );

  $form['name'] = array(
    '#type' => 'machine_name',
    '#description' => t('The name is used in URLs for generated images. Use only lowercase alphanumeric characters, underscores (_), and hyphens (-).'),
    '#size' => '64',
    '#required' => TRUE,
    '#default_value' => $defaults['name'],
    '#disabled' => isset($style),
    '#machine_name' => array(
      'exists' => 'assetbank_image_style_load',
      'source' => array('label'),
      'replace_pattern' => '[^0-9a-z_\-]',
      'error' => t('Please only use lowercase alphanumeric characters, underscores (_), and hyphens (-) for style names.'),
    ),
  );

  $form['width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => $defaults['width'],
    '#field_suffix' => ' ' . t('pixels'),
    '#required' => TRUE,
    '#size' => 10,
    '#allow_negative' => FALSE,
  );

  $form['height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#default_value' => $defaults['height'],
    '#field_suffix' => ' ' . t('pixels'),
    '#required' => TRUE,
    '#size' => 10,
    '#allow_negative' => FALSE,
  );

  $form['preserve_aspect'] = array(
    '#type' => 'checkbox',
    '#title' => t('Preserve aspect ratio'),
    '#default_value' => $defaults['preserve_aspect'],
    '#description' => t('When an image is resized, this will ensure the image is not sqeeze or stretched to match the dimensions specified above.'),
  );

  $form['crop_to_fit'] = array(
    '#type' => 'checkbox',
    '#title' => t('Crop to fit'),
    '#default_value' => $defaults['crop_to_fit'],
    '#description' => t('When an image is resized to fit the dimensions above, the image can optionally be cropped to make the aspect ratio match those of the specified dimensions.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="preserve_aspect"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['fill_background'] = array(
    '#type' => 'checkbox',
    '#title' => t('Fill background with color'),
    '#default_value' => $defaults['fill_background'],
    '#description' => t('When an image is resized without cropping, blank areas may appear in the image. You can specify the color of the blank areas here.'),
    '#states' => array(
      'visible' => array(
        ':input[name="crop_to_fit"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['fill_background_hexcode'] = array(
    '#type' => 'textfield',
    '#title' => t('Background fill color'),
    '#default_value' => $defaults['fill_background_hexcode'],
    '#size' => 6,
    '#maxlength' => 6,
    '#description' => t('The hex triplet representing the background color for resized images. Omit hash prefix. Example: F0F0F0'),
    '#states' => array(
      'visible' => array(
        ':input[name="fill_background"]' => array('checked' => TRUE),
        ':input[name="crop_to_fit"]' => array('checked' => FALSE),
      ),
      'required' => array(
        ':input[name="fill_background"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save style'),
  );

  return $form;
}

/**
 * Validation handler for AssetBank Image Style adding/editing form.
 */
function assetbank_image_style_form_validate($form, &$form_state) {
  // Check pixel fields are numeric.
  if (!is_numeric($form_state['values']['width'])) {
    form_set_error('width', 'Pixel value must be numeric.');
  }

  if (!is_numeric($form_state['values']['height'])) {
    form_set_error('height', 'Pixel value must be numeric.');
  }

  // Check to see background color is set if fill option is enabled.
  if ($form_state['values']['fill_background'] === 1) {
    if (!ctype_xdigit($form_state['values']['fill_background_hexcode'])) {
      form_set_error('fill_background_hexcode', 'Background color must be a valid hex triplet.');
    }
  }
}

/**
 * Submit handler for AssetBank Image Style adding/editing form.
 */
function assetbank_image_style_form_submit($form, &$form_state) {
  $style = array(
    'name' => $form_state['values']['name'],
    'label' => $form_state['values']['label'],
    'width' => $form_state['values']['width'],
    'height' => $form_state['values']['height'],
    'preserve_aspect' => $form_state['values']['preserve_aspect'],
    'crop_to_fit' => $form_state['values']['crop_to_fit'],
    'fill_background' => $form_state['values']['fill_background'],
    'fill_background_hexcode' => $form_state['values']['fill_background_hexcode'],
  );

  $styles = variable_get('assetbank_image_image_styles');

  $styles[$style['name']] = $style;

  variable_set('assetbank_image_image_styles', $styles);

  drupal_set_message(t('Style %name was created.', array('%name' => $style['label'])));
  $form_state['redirect'] = 'admin/config/media/assetbank/assetbank-image-styles';
}

/**
 * Form builder; form for deleting an AssetBank Image Style.
 *
 * @ingroup forms
 * @see assetbank_image_style_delete_form_submit()
 */
function assetbank_image_style_delete_form($form, &$form_state, $style) {
  $form_state['assetbank_image_style'] = $style;

  return confirm_form(
    $form,
    t('Are you sure you want to delete %style?', array('%style' => $style['label'])),
    'admin/config/media/assetbank/assetbank-image-styles',
    NULL,
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler to delete an AssetBank Image Style.
 */
function assetbank_image_style_delete_form_submit($form, &$form_state) {
  $style = $form_state['assetbank_image_style'];

  $styles = variable_get('assetbank_image_image_styles');

  unset($styles[$style['name']]);

  variable_set('assetbank_image_image_styles', $styles);

  drupal_set_message(t('Style %name was deleted.', array('%name' => $style['label'])));
  $form_state['redirect'] = 'admin/config/media/assetbank/assetbank-image-styles';
}

/**
 * Returns HTML for the page containing the list of AssetBank Image Styles.
 *
 * @param array $variables
 *   An associative array containing:
 *   - styles: An array of all the image styles.
 *
 * @see assetbank_image_get_styles()
 * @ingroup themeable
 */
function theme_assetbank_image_style_list($variables) {
  $styles = $variables['styles'];

  $header = array(
    t('Style name'),
    t('Width'),
    t('Height'),
    array(
      'data' => t('Operations'),
      'colspan' => 2,
    ),
  );
  $rows = array();
  foreach ($styles as $style) {
    $row = array();
    $row[] = l($style['label'], 'admin/config/media/assetbank/assetbank-image-styles/edit/' . $style['name']);
    $row[] = $style['width'] . ' px';
    $row[] = $style['height'] . ' px';

    $row[] = l(t('edit'), 'admin/config/media/assetbank/assetbank-image-styles/edit/' . $style['name']);
    $row[] = l(t('delete'), 'admin/config/media/assetbank/assetbank-image-styles/delete/' . $style['name']);
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(
      array(
        'colspan' => 4,
        'data' => t('There are currently no styles. <a href="!url">Add a new one</a>.', array('!url' => url('admin/config/media/assetbank/assetbank-image-styles/add'))),
      ),
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Menu callback listing of all AssetBank instances.
 */
function assetbank_instance_list() {
  $page = array();

  $instances = assetbank_instances();
  $page['assetbank_instance_list'] = array(
    '#markup' => theme('assetbank_instance_list', array('instances' => $instances)),
  );

  return $page;
}

/**
 * Form builder; form for adding a new AssetBank Instance.
 *
 * @ingroup forms
 */
function assetbank_instance_form($form, &$form_state, $instance = NULL) {
  if (isset($form_state['values'])) {
    $defaults = $form_state['values'];
  }
  elseif ($instance) {
    $defaults = $instance;
    $form_state['assetbank_instance'] = $instance;
  }
  else {
    $defaults = array(
      'label' => '',
      'name' => '',
      'url' => '',
    );
  }

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Instance name'),
    '#default_value' => $defaults['label'],
    '#disabled' => isset($style),
    '#required' => TRUE,
  );

  $form['name'] = array(
    '#type' => 'machine_name',
    '#description' => t('The machine name for this AssetBank instance'),
    '#size' => '64',
    '#required' => TRUE,
    '#default_value' => $defaults['name'],
    '#disabled' => isset($instance),
    '#machine_name' => array(
      'exists' => 'assetbank_instance_load',
      'source' => array('label'),
      'replace_pattern' => '[^0-9a-z_\-]',
      'error' => t('Please only use lowercase alphanumeric characters, underscores (_), and hyphens (-) for style names.'),
    ),
  );

  $form['url'] = array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#default_value' => $defaults['url'],
    '#required' => TRUE,
    '#size' => 100,
    '#allow_negative' => FALSE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save style'),
  );

  return $form;
}

/**
 * Validation handler for AssetBank Instance adding/editing form.
 */
function assetbank_instance_form_validate($form, &$form_state) {
  // Validate URL.
  if (!filter_var($form_state['values']['url'], FILTER_VALIDATE_URL)) {
    form_set_error('url', 'URL must be valid.');
  }

}

/**
 * Submit handler for AssetBank Image Style adding/editing form.
 */
function assetbank_instance_form_submit($form, &$form_state) {
  $instance = array(
    'name' => $form_state['values']['name'],
    'label' => $form_state['values']['label'],
    'url' => $form_state['values']['url'],
  );

  $instances = variable_get('assetbank_image_instances');

  $instances[$instance['name']] = $instance;

  variable_set('assetbank_image_instances', $instances);

  drupal_set_message(t('Instance %name was updated.', array('%name' => $instance['label'])));
  $form_state['redirect'] = 'admin/config/media/assetbank';
}

/**
 * Form builder; form for deleting an AssetBank Instance.
 *
 * @param array $instance
 *   An array describing the instance to be deleted.
 *
 * @ingroup forms
 * @see assetbank_instance_delete_form_submit()
 */
function assetbank_instance_delete_form($form, &$form_state, $instance) {
  $form_state['assetbank_instance'] = $instance;

  return confirm_form(
    $form,
    t('Are you sure you want to delete %instance?', array('%instance' => $instance['label'])),
    'admin/config/media/assetbank',
    NULL,
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler to delete an AssetBank Image Style.
 */
function assetbank_instance_delete_form_submit($form, &$form_state) {
  $instance = $form_state['assetbank_instance'];

  $instances = variable_get('assetbank_image_instances');

  unset($instances[$instance['name']]);

  variable_set('assetbank_image_instances', $instances);

  drupal_set_message(t('Instance %name was deleted.', array('%name' => $instance['label'])));
  $form_state['redirect'] = 'admin/config/media/assetbank';
}

/**
 * Returns HTML for the page containing the list of AssetBank Image Styles.
 *
 * @param array $variables
 *   An associative array containing:
 *   - instances: An array of all instances.
 *
 * @see assetbank_image_get_styles()
 * @ingroup themeable
 */
function theme_assetbank_instance_list($variables) {
  $instances = $variables['instances'];

  $header = array(
    t('Instance name'),
    t('URL'),
    array(
      'data' => t('Operations'),
      'colspan' => 2,
    ),
  );
  $rows = array();
  foreach ($instances as $instance_name => $instance) {
    $row = array();
    $row[] = l($instance['label'], 'admin/config/media/assetbank/edit/' . $instance_name);
    $row[] = $instance['url'];
    $row[] = l(t('edit'), 'admin/config/media/assetbank/edit/' . $instance_name);
    $row[] = l(t('delete'), 'admin/config/media/assetbank/delete/' . $instance_name);
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(
      array(
        'colspan' => 4,
        'data' => t('There are currently no instances. <a href="!url">Add a new one</a>.', array('!url' => url('admin/config/media/assetbank/add'))),
      ),
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}
