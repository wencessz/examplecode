<?php

class RoiWidget{

  public static function initialize_variables(&$variables){
    $variables['filters'] = "";
    $variables['insights_section'] = "";
    $variables['filters_legend'] = "";
    $variables['field_button_open_legend'] = "";
    $variables['insight_integrated_section'] = "";
    $variables['timeline_section'] = "";
    $variables['press_awards_section'] = "";
    $variables['filter_year_label'] = "";
    $variables['year_label'] = "";
  }

  private static function get_view_insights_section(&$variables,$wrapper){ //this is private because there is no validations here, so this only should be called with correct previous validations
    $display = 'default';
    if(drupal_is_front_page() || (RoiMisc::node_has_type($variables['node'],'page') 
        && RoiMisc::getEntityWrapperPropertyValue($wrapper, 'field_page_type') == 'homepage')){
      $display = 'homepage';
    }
    if(RoiMisc::node_has_type($variables['node'],'integrated_solution')){
      $display = 'integrated_solutions';
    }
    if(RoiMisc::node_has_type($variables['node'],'supplier_partner')){
      $display = 'supplier_partners';
    }
    if(RoiMisc::node_has_type($variables['node'],'page') && RoiMisc::getEntityWrapperPropertyValue($wrapper, 'field_page_type') == 'supplier'){
      $display = 'supplier_partners_section';
    }
    if(RoiMisc::node_has_type($variables['node'],'page') && RoiMisc::getEntityWrapperPropertyValue($wrapper, 'field_page_type') == 'integrated'){
      $display = 'integrated_solutions_sections';
    }
    if(RoiMisc::node_has_type($variables['node'],'page') && RoiMisc::getEntityWrapperPropertyValue($wrapper, 'field_page_type') == 'insights'){
      $display = 'insights';
    }
    self::get_view($variables,'insights',$display);
  }

  private static function get_view(&$variables,$viewname,$display){
    $view = views_get_view($viewname);
    $view->set_display($display);
    $view->pre_execute();
    $view->execute();
    $variables[$viewname.'_section'] = $view->render();
  }

  public static function get_insights_section(&$variables,$wrapper) {
    
    $field_page_type_value = RoiMisc::getEntityWrapperPropertyValue($wrapper, 'field_page_type');
    if(!empty($field_page_type_value)){
      if($field_page_type_value == 'press' ||
        $field_page_type_value == 'history'){
        return;
      }
    }
    $field_widget_options_value = $wrapper->field_widget_options->value();
    if($variables['view_mode'] == 'full' && !empty($field_widget_options_value) && is_a($wrapper->field_widget_options, 'EntityStructureWrapper')){
      $filter_enabled = RoiMisc::getEntityWrapperPropertyValue($wrapper->field_widget_options, 'field_options');
      $field_widget = RoiMisc::getEntityWrapperPropertyValue($wrapper->field_widget_options, 'field_widget');
      if(!empty($field_widget) && in_array('insights_section', $field_widget)){
        self::get_view_insights_section($variables,$wrapper);
        $insights_section = $variables['insights_section'];
        $variables['insights_section'] = array(
          '#theme' => 'roi_insights_section',
          '#button_label' => '',
          '#title_legend' => '',
          '#open_legend' => '',
          '#filter_form' => '',
          '#insights_section' => '',
        );
        $variables['insights_section']['#insights_section'] = $insights_section;
        if(in_array('filter_integrated_solution_enabled', $filter_enabled)){
          $variables['insights_section']['#button_label'] = RoiMisc::getEntityWrapperPropertyValue($wrapper, 'field_widget_options->field_button_label_popup');
          $variables['insights_section']['#title_legend'] = $wrapper->field_widget_options->field_title_legend->value();
          $variables['insights_section']['#open_legend'] = $wrapper->field_widget_options->field_button_open_legend->value();
          $variables['insights_section']['#filter_form'] = drupal_get_form("insights_filter_form");
        }
      }
    }
  }

  public static function get_press_awards_section(&$variables,$wrapper) {
    $field_page_type_value = $wrapper->field_page_type->value();
    if($variables['view_mode'] == 'full' && $field_page_type_value == 'press'){
      self::get_view($variables,'press_awards','default');
    }
  }

  public static function get_timeline_section(&$variables,$wrapper) {
    $field_page_type_value = $wrapper->field_page_type->value();
    if($variables['view_mode'] == 'full' && $field_page_type_value == 'history' && is_a($wrapper->field_widget_options, 'EntityStructureWrapper')){
      self::get_view($variables,'timeline','default');
    }
  }

  public static function get_insight_integrated_section(&$variables,$wrapper) {
    $field_widget_options_value = $wrapper->field_widget_options->value();
      if($variables['view_mode'] == 'full' && !empty($field_widget_options_value) && is_a($wrapper->field_widget_options, 'EntityStructureWrapper')){
        if(in_array('insight_integrated_section', $wrapper->field_widget_options->field_widget->value())){
          $elements = $wrapper->field_widget_options->field_rel_integrated_insights->value();
          $to_render_elements = array();
          foreach ($elements as $element) {
            $to_render_elements[] = node_view($element, $view_mode = 'widget_carrousel');
          }
          $variables['insight_integrated_section'] = theme('roi_insight_integrated_section', array('elements' => $to_render_elements, 'title' => $wrapper->field_widget_options->field_integrated_insights_title->value()));
        }
      }
  }

  public static function heading_block(&$variables,$wrapper) {
    $field_header_value = $wrapper->field_header->value();
    if(!empty($field_header_value)){
      $images_style = 'insight_widget';
      if(!empty($variables['view_mode'])){
        if($variables['view_mode'] == 'related'){
          $images_style = 'custom_thumbnail';
        }
      }

      $field_thumbnail_value = $wrapper->field_header->field_thumbnail->value();

      $variables['custom_background'] = '';
      $variables['custom_field_header_size']  = '';
      $variables['custom_field_header_size'] = RoiWidget::get_header_size($wrapper);
      $variables['custom_field_short_description'] = check_markup($wrapper->field_header->field_short_description->value(), 'full_html');
      $variables['custom_field_thumbnail'] = !empty($field_thumbnail_value['uri']) ? image_style_url($images_style, $field_thumbnail_value['uri']) : '';
      $variables['custom_field_thumbnail_small'] = !empty($field_thumbnail_value['uri']) ? image_style_url('is_insight_section', $field_thumbnail_value['uri']) : '';
      $variables['custom_field_header_color'] = $wrapper->field_header->field_header_color->value();
      $field_header_image_element =  $wrapper->field_header->field_header_image->value();
      $variables['custom_use_image'] = !empty($field_header_image_element['uri']) ? TRUE : FALSE;
      $variables['custom_field_header_image'] = !empty($field_header_image_element['uri']) ? image_style_url('heading_image', $field_header_image_element['uri']) : '';
      $variables['custom_field_headline'] = check_markup($wrapper->field_header->field_headline->value(), 'full_html');
      $variables['custom_field_header_link'] = $wrapper->field_header->field_header_link->raw();
      $field_header_video = $wrapper->field_header->field_header_video->value();
      $variables['video_player'] = '';
      $variables['video_player_id'] = '';
      $variables['video_modal'] = '';
      if(!empty($field_header_video['file']) && !empty($field_header_video['file']->filemime)){
        switch ($field_header_video['file']->filemime) {
          case 'video/youtube':
            $media_displays = media_youtube_file_default_displays();
            $options = $media_displays['video__default__media_youtube_video']->settings;
            $options['width'] = '100%';
            $options['height'] = '100%';
            $options['captions'] = '100%';
            $variables['video_player'] = theme('media_youtube_video', array('uri' => $field_header_video['file']->uri,'options' => $options));
            break;
          case 'video/vimeo':
            $media_displays = media_vimeo_file_default_displays();
            $options = $media_displays['video__default__media_vimeo_video']->settings;
            $options['width'] = '100%';
            $options['height'] = '100%';
            $variables['video_player'] = theme('media_vimeo_video', array('uri' => $field_header_video['file']->uri,'options' => $options));
            break;
          default:
            $video_attrs = array(
              'src' => file_create_url($field_header_video['file']->uri),
              'width' => '100%',
              'height' => '100%'
            );
            $variables['video_player'] = theme('mediaelement_video', array('attributes' => $video_attrs, 'settings' => array('download_link' => FALSE ) ));
            break;
        }
        $variables['video_player_id'] = 'modal-' . uniqid();
        $variables['video_modal'] = theme('roi_modal_video', array('player' => $variables['video_player'], 'player_id' => $variables['video_player_id']));
      }
    }
  }

  public static function widget_paragraph_heading(&$variables, $wrapper){
    $variables['custom_background'] = '';
    $variables['custom_field_header_size'] = 'small_header';
    $variables['custom_field_short_description'] = $wrapper->field_section_short_description->value();
    $variables['custom_field_header_color'] = $wrapper->field_header_color->value();
    $field_header_image_element =  $wrapper->field_section_image->value();
    $variables['custom_use_image'] = !empty($field_header_image_element['uri']) ? TRUE : FALSE;
    $variables['custom_field_header_image'] = !empty($field_header_image_element['uri']) ? image_style_url('heading_image', $field_header_image_element['uri']) : '';
    $variables['custom_field_headline'] = $wrapper->field_section_title->value();
    $variables['custom_field_section_button_label'] = $wrapper->field_section_button_label->value();
    $variables['modal_id'] = 'modal-' . uniqid();
  }

  public static function get_header_size($wrapper){
    $field_header_image_element =  $wrapper->field_header->field_header_image->value();
    if(drupal_is_front_page()){
      return 'large_header';
    }
    if(!empty($field_header_image_element['uri'])){
      return 'large_header';
    }
    return 'small_header';
  }
}
