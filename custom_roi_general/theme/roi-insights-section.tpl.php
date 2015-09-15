  <?php if(!empty($filter_form)): ?>
  <div class="container">
    <div class="content-main filters filters-wrapper">
      <div class="panel-group" id="accordion">
        <div class="panel panel-default filters insight-filters">
          <div class="panel-heading" id="headingOne">
            <h4 class="panel-title text-center">
              <em><?php print $open_legend; ?></em>
              <a href="#"  class="btn show-filters">
                <div class="circle-icon"><span class="glyphicon glyphicon-menu-down"></span></div>
                <?php print $button_label; ?>
              </a>
            </h4>
            <h5 class="panel-title-hidden">
              <p class="text-center">
                <span class="label">
                  <?php print $title_legend; ?>
                </span>
                <button type="button" class="close"><span>&times;</span></button>
              </p>
            </h5>
          </div>
          <div id="insights" class="panel-collapse collapse">
            <div class="panel-body">
              <?php print drupal_render($filter_form); ?>
            </div>
          </div>
        </div>
      </div> <!-- .panel-group-->
    </div>
  </div>
<?php endif; ?>
  <?php print $insights_section; ?>
