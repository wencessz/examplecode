<?php

class RoiMisc {
  /**
   * Check whether the current entity is a node or not.
   * @param $node
   * @return bool
   */
  public static function is_node($node) {
    if (!empty($node->nid)) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Check whether the node has a specific property or not.
   * @param $node
   * @param $property
   * @return bool
   */
  public static function node_has_property($node, $property) {
    $wrapper = entity_metadata_wrapper('node', $node);
    $fields = $wrapper->getPropertyInfo();
    if (!empty($fields[$property])) {
      $field = $wrapper->{$property}->value();
      if (!empty($field)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  /**
   * Check whether the node has a specified type or not.
   * @param $node
   * @param $type
   * @return bool
   */
  public static function node_has_type($node, $type) {
    if (property_exists($node, 'type') && $node->type == $type) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Returns whether the insight is a chart or not
   * @param $wrapper
   * @return bool
   */
  public static function insight_is_chart($wrapper) {
    if ($wrapper->field_insight_type->value() == 'micro_content' && $wrapper->field_micro_content_type->value() == 'chart') {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Returns whether the insight is a fact or not
   * @param $wrapper
   * @return bool
   */
  public static function insight_is_fact($wrapper) {
    if ($wrapper->field_insight_type->value() == 'micro_content' && $wrapper->field_micro_content_type->value() == 'fact') {
      return TRUE;
    }
    return FALSE;
  }

  public static function get_current_node_insight_rows() {
    $object = menu_get_object();
    if (empty($object->nid)) {
      return NULL;
    }
    $wrapper = entity_metadata_wrapper('node', $object);
    $widget_options = RoiMisc::getEntityWrapperPropertyValue($wrapper, 'field_widget_options');
    if (empty($widget_options)) {
      return NULL;
    }

    if (!empty($widget_options->field_number_rows[LANGUAGE_NONE][0]['value'])) {
      return $widget_options->field_number_rows[LANGUAGE_NONE][0]['value'];
    }

    return NULL;

  }

  /**
   * Retrieves the next position of the fact contained within the $items array based
   * on the given current item position.
   *
   * @param $items
   * @param $current_key
   * @return array
   */
  public static function get_next_fact($items, $current_key){
    $next_item = array();
    foreach($items as $key => $item){
      if($key != $current_key){
        if($items[$key]['type'] == 'fact'){
          $next_item['item'] = $items[$key];
          $next_item['key'] = $key;
          break;
        }
      }
    }
    return $next_item;
  }

  /**
   * Order the view result based on the TOKY layout. (Row: Case of Study / Article - Chart - Fact)
   * @param $variables
   * @return array
   */
  public static function custom_insights_order($variables) {
    $view = $variables['view'];
    $variables['custom_rows'] = array();

    //Each type must have a bootstrap column length (*1.5 will cause that the
    //item will try to fill one
    //column of col-*-3 size)
    $length_cols = array(
      'case_study' => 6,
      'chart' => 3,
      'fact' => 1.5,
      'article' => 6
    );
    $total_row_size = 12;
    $current_node_insight_rows = self::get_current_node_insight_rows();
    $num_rows = ($current_node_insight_rows) ? $current_node_insight_rows : 3;

    $group_option_items = array();
    $insights = array();
    $num_types = count($length_cols);

    //Get the indexes from the view result based on the insight types.
    foreach ($view->result as $item) {
      $node = node_load($item->nid);
      $wrapper = entity_metadata_wrapper('node', $node);
      if (RoiMisc::insight_is_chart($wrapper)) {
        $insight_type[] = 'chart';
      }
      else {
        if (RoiMisc::insight_is_fact($wrapper)) {
          $insight_type[] = 'fact';
        }
        else {
          $insight_type[] = $wrapper->field_insight_type->value();
        }
      }
    }

    //Clone the insights types that contains the information of indexes.
    $group_option_items = $insight_type;

    for ($i = 1; $i <= $num_rows; $i++) {
      if (count($group_option_items) == 0) {
        break;
      }
      $row_size = 0;
      $prev_element_type = '';
      //Odd and Even
      $odd = ($i % 2 == 0);

      //reset the types to seek on each iteration
      $types_to_search = $length_cols;

      //according to the num of types, lets make an iteration
      //performing an array search by insight type
      for ($c = 0; $c < $num_types; $c++) {
        if (count($group_option_items) == 0) {
          break;
        }
        foreach ($types_to_search as $type => $col_value) {
          if (count($group_option_items) == 0) {
            break;
          }
          //if the available type is within the array,
          //lets try to push it into the row
          $key_cs = array_search($type, $group_option_items);
          if ($key_cs !== FALSE) {

            //Only push the type if the length is less or equal
            //than the bootstrap full row width (12)
            if (($row_size + $col_value) <= $total_row_size) {
              $index = $view->result[$key_cs]->nid;
              $variables['custom_rows'][$i][$index]['rendered'] = $variables['rows'][$key_cs];
              $variables['custom_rows'][$i][$index]['type'] = $type;
              $row_size += $length_cols[$type];
              unset($group_option_items[$key_cs]);
              $prev_element_type = $type;
            }
          }
        }

        //If we completed the row length on the iteration, lets skip to
        //the next row.
        if ($row_size >= $total_row_size) {
          break;
        }
      }

      //If row is odd, lets try to reverse the array to pull to the right
      //the 6 cols item
      if ($odd) {
        $variables['custom_rows'][$i] = array_reverse($variables['custom_rows'][$i], TRUE);
      }
    }

    return $variables['custom_rows'];
  }

  /**
   * Order the view result based on the TOKY layout. (Row: Case of Study / Article - Chart - Fact)
   * @param $field
   * @param $nodes
   * @param $query
   */
  public static function custom_add_filter_related($field, $nodes, &$query) {
    if (count($nodes)) {
      $query->add_table('field_data_' . $field);
      $db_or = db_or();
      foreach ($nodes as $node) {
        $db_or->condition('field_data_' . $field . '.' . $field . '_target_id', $node->nid, '=');
      }
      $query->add_where(0, $db_or);
    }
  }

  /**
   * Order the view result based on the TOKY layout. (Row: Case of Study / Article - Chart - Fact)
   * @param $field
   * @param $nodes
   * @param $query
   */
  public static function custom_add_filters_related($fields, &$query) {
    if (count($fields)) {
      $db_or = db_or();
      foreach ($fields as $field => $nodes) {
        $query->add_table('field_data_' . $field);
        foreach ($nodes as $node) {
          $db_or->condition('field_data_' . $field . '.' . $field . '_target_id', $node->nid, '=');
        }
      }
      $query->add_where(0, $db_or);
    }
  }

  public static function truncate($string, $length, $stop_anywhere = FALSE, $ellipsis = TRUE) {
    //truncates a string to a certain char length, stopping on a word if not specified otherwise.
    if (strlen($string) > $length) {
      $total_length = $length;
      if ($ellipsis) {
        $total_length = $length - 3;
      }
      $string = substr($string, 0, $total_length);

      if ($stop_anywhere) {
        //stop anywhere
        if ($ellipsis) {
          $string .= '...';
        }
      }
      else {
        //stop on a word.
        $string = substr($string, 0, strrpos($string, ' '));
        if ($ellipsis) {
          $string .= '...';
        }
      }
    }
    return $string;
  }

  /**
   * Returns the property value from the given EntityStructureWrapper
   * It's posible use "->" as delimiter to get children properties. Useful to
   * get properties from referenced entities.
   *
   * @param EntityStructureWrapper $wrapper Wrapper
   * @param string $property property name
   * @param string $filter_id
   * @return mixed The value
   */
  public static function getEntityWrapperPropertyValue(EntityStructureWrapper $wrapper, $property, $filter_id = 'full_html') {
    try {
      $fields = explode('->', $property);
      $prop = $wrapper->get(array_shift($fields));
      if (is_a($prop, 'EntityStructureWrapper') && count($fields)) {
        $value = RoiMisc::getEntityWrapperPropertyValue($prop, implode('->', $fields));
      }
      else {
        $value = $prop->value();
      }
    }
    catch (EntityMetadataWrapperException $exc) {
      $value = '';
    }

    if( gettype($value) == 'string' ){
      return check_markup($value, $filter_id);
    }

    return $value;
  }

  /**
   * Sets the default value for field_case_of_study
   * @param StdClass $node
   * @return StdClass node formated
   */
  public static function insightPrepareCaseStudy($node) {
    if (!empty($node) && empty($node->field_case_of_study)) {
      $pid = variable_get('roi_conf_mdcsc_paragraph', 0);
      if ($pid) {
        $node->field_case_of_study = array(
          LANGUAGE_NONE => array(
            array(
              'value' => $pid,
              'revision_id' => $pid
            )
          )
        );
      }
    }
    return $node;
  }

  public static function getDefaultPage($type) {
    $pages = &drupal_static(__FUNCTION__);
    if (isset($pages[$type])) {
      return $pages[$type];
    }
    $pages[$type] = 0;

    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'page')
      ->propertyCondition('status', NODE_PUBLISHED)
      ->fieldCondition('field_page_type', 'value', $type)
      ->propertyOrderBy('created', 'DESC')
      ->range(0, 1);

    $result = $query->execute();
    if (isset($result['node'])) {
      $nids = array_keys($result['node']);
      $pages[$type] = $nids[0];
    }
    return $pages[$type];
  }

  /**
   * Verify if the path has header
   * @param type $path
   * @return type
   */
  public static function path_has_header($path = NULL) {
    $paths = array('benefits');
    if (empty($path)) {
      $path = current_path();
    }
    return in_array($path, $paths);
  }

  /**
   * Verify if the node has header
   * @param StdClass $node
   * @return bool
   */
  public static function node_has_header($node) {
    $types = array('career_page', 'testimonial','press_awards');
    if (self::node_has_property($node, 'field_header')) {
      return TRUE;
    }
    elseif ((isset($node->type) && in_array($node->type, $types)) || !empty($node->parent_nid)) {
      return TRUE;
    }
    return FALSE;
  }


  /**
   * Verify if the node needs to show the contact us footer
   * @param StdClass $node
   * @return bool
   */
  public static function node_has_contact_footer($node) {
    $wrapper = entity_metadata_wrapper('node', $node);
    $contact_footer = self::getEntityWrapperPropertyValue($wrapper, 'field_widget_options->field_enable_contact_footer');
    return !empty($contact_footer);
  }


  public static function validate_paragraphs_submit($bundle, $fields, $paragraph, $wrapper) {
    foreach ($fields as $field_name) {
      $field_info = field_info_instance("paragraphs_item", $field_name, $bundle);
      $field_paragraph_name = $paragraph->field_name;
      $field_value = RoiMisc::getEntityWrapperPropertyValue($wrapper->$field_paragraph_name, $field_name);
      if(empty($field_value)){
        form_set_error($field_paragraph_name."][".LANGUAGE_NONE."][0][".$field_name."][".LANGUAGE_NONE."][0][value", $field_info['label'].' field is required.');
      }
    }
  }

  public static function validate_node_submit($bundle, $fields, $wrapper){
    foreach ($fields as $field_name) {
      $field_info = field_info_instance("node",$field_name,$bundle);
      $field_value = RoiMisc::getEntityWrapperPropertyValue($wrapper, $field_name);
      if(empty($field_value)){
        form_set_error($field_name, $field_info['label'] . ' field is required.');
      }
      elseif (!empty($field_info)) {
        $field = field_info_field($field_name);
        switch ($field['type']) {
          case 'link_field':
            if (empty($field_value['url'])) {
              form_set_error($field_name, $field_info['label'] . ' field is required.');
            }
            break;
          case 'image':
            if (empty($field_value['fid'])) {
              form_set_error($field_name, $field_info['label'] . ' field is required.');
            }
            break;
          case 'entityreference':
            $empty = TRUE;
            foreach ($field_value as $item) {
              if(!empty($item) && is_object($item)){
                $empty = FALSE;
                break;
              }
            }
            if ($empty) {
              form_set_error($field_name, $field_info['label'] . ' field is required.');
            }
            break;
          default:
            break;
        }
      }
    }
  }

  public static function node_get_parent_page($node){
    $pageTypes = array(
      'insight' => 'insights',
      'benefit' => 'insights',
      'supplier' => 'insights',
      'leader' => 'leadership',
      'integrated_solution' => 'integrated',
      'supplier_partner' => 'supplier',
      'contact_office' => 'contact',
    );
    $nid = 0;
    if(!empty($pageTypes[$node->type])){
      $nid = self::getDefaultPage($pageTypes[$node->type]);
    }
    return $nid;
  }

  public static function check_markup_array(&$elements, $filter_id = 'full_html'){
    foreach ($elements as $key => $element) {
      $type = gettype($elements[$key]);
      if($type == 'array') {
        $elements[$key] = self::check_markup_array($element, $filter_id);
      }
      else {
        $elements[$key] = check_markup($element, $filter_id);
      }
    }
    return $elements;
  }

  public static function check_format_charts($elements){
    foreach ($elements as $key => $element) {
      $type = gettype($elements[$key]);
      if($type == 'array') {
        $elements[$key] = self::check_format_charts($element);
      }
      else {
        $elements[$key] = self::chart_format_data($element);
      }
    }
    return $elements;
  }

  public static function chart_format_data($text, $check_markup = FALSE, $filter_id = 'full_html'){
    $text = str_replace('"', '', $text);
    $text = str_replace("'", '', $text);
    $text = htmlspecialchars($text);
    if( $check_markup ) {
      $text = check_markup($text, $filter_id);
    }
    return $text;
  }
}

