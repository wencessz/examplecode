<?php
/**
 * Impements hook_preprocess_HOOK
 *
 * @see custom_roi_general_theme()
 * @see theme_roi_contact()
 */
function custom_roi_general_preprocess_roi_contact(&$variables){
  $settings = variable_get('roi_conf_footer', array());
  foreach ($variables['info'] as $key => $value) {
    if(empty($value) && !empty($settings['contact_info'][$key])) {
      $variables['info'][$key] = $settings['contact_info'][$key];
    }
  }
}
/**
 * Impements hook_preprocess_HOOK
 *
 * @see custom_roi_general_theme()
 * @see theme_roi_contact()
 */
function custom_roi_general_preprocess_roi_contact_footer(&$variables){
  if(!empty($variables['bg_color']) && !strpos($variables['bg_color'], '_')){
    $variables['bg_color'] = $variables['bg_color'] . '_1';
  }
}

/**
 * Returns HTML for ROi logo.
 *
 * @param $variables
 *   An associative array containing:
 *   - width: Logo width, the default value is 100
 *   - height: Logo height, the default value is 100
 *
 * @ingroup themeable
 */
function theme_roi_logo($variables){
  $attributes = '';
  if(!empty($variables['attributes'])){
    if(isset($variables['attributes']['width'])){
      unset($variables['attributes']['width']);
    }
    if(isset($variables['attributes']['height'])){
      unset($variables['attributes']['height']);
    }
    $attributes = drupal_attributes($variables['attributes']);
  }
  
  $logo = '
  <svg !attributes xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" id="Layer_1" x="0px" y="0px" width="@width" height="@height" viewBox="0 0 89.854 89.852" enable-background="new 0 0 89.854 89.852" xml:space="preserve">
    <rect fill="#00a1e1" width="89.854" height="89.852" />
    <g>
      <path fill="#FFFFFF" d="M36.143,50.648c0-2.165-0.828-4.284-2.242-5.779c-1.535-1.612-3.459-2.322-6.448-2.322H16.875v26.225h6.802   V58.707l6.486,10.064h8.494l-8.259-10.733C34.134,57.173,36.143,54.582,36.143,50.648z M24.936,54.304h-1.259v-6.525h1.259   c2.635,0,4.207,1.216,4.207,3.265C29.143,53.086,27.605,54.304,24.936,54.304z" />
      <rect x="66.961" y="51.312" fill="#FFFFFF" width="6.167" height="17.46" />
      <path fill="#FFFFFF" d="M50.998,42.291c-8.13,0-14.221,5.899-14.221,13.686c0,7.824,6.091,13.724,14.221,13.724   c8.136,0,14.227-5.899,14.227-13.724C65.225,48.149,59.134,42.291,50.998,42.291z M50.998,63.224c-4.01,0-7.244-3.238-7.244-7.247   c0-3.971,3.234-7.211,7.244-7.211c4.014,0,7.246,3.24,7.246,7.211C58.244,59.985,55.012,63.224,50.998,63.224z" />
      <rect x="66.947" y="41.091" fill="#FFFFFF" width="6.138" height="6.135" />
    </g>
  </svg>
  ';
  $logo = l($logo,'<front>',array('html' => TRUE));
  return format_string($logo, array(
    '@width' => !empty($variables['width']) ? $variables['width'] : '100',
    '@height' => !empty($variables['height']) ? $variables['height'] : '100',
    '!attributes' => $attributes,
  ));
}

/**
 * Returns HTML for ROi logo.
 *
 * @param $variables
 *   An associative array containing:
 *   - info: An array with the following information:
 *     - address
 *     - city
 *     - zip_code
 *     - state
 *     - phone
 *     - copyright
 *
 * @ingroup themeable
 */
function theme_roi_contact(&$variables){
  $info = array();
  $contact = $variables['info'];
  if(!empty($contact['address'])){
    $info[] = $contact['address'];
  }
  $city = array();
  if(!empty($contact['city'])){
    $city[] = $contact['city'];
  }
  if(!empty($contact['state'])){
    if($contact['zip_code']) {
      $contact['state'] .= " {$contact['zip_code']}";
    }
    $city[] = $contact['state'];
  }
  if(!empty($city)) {
    $info[] = implode(", ", $city);
  }
  if(!empty($contact['phone'])){
    $info[] = t('Phone') . ": {$contact['phone']}";
  }
  if(!empty($contact['copyright'])){
    $info[] = theme('html_tag', array(
      'element' => array(
        '#tag' => 'span',
        '#value' => $contact['copyright']
    )));
  }
  return theme('html_tag', array(
    'element' => array(
      '#tag' => 'address',
      '#value' => implode("<br>", $info)
  )));
}
