<?php
$background = '';
if(!empty($image)){
  $background = 'background: url(' . $image . ') no-repeat center;';
}
?>
<div class="jumbotrSolving on header-heavy jumbotrSolving-layout2 header-md <?php print empty($class) ? '' : $class;  ?>" style="<?php echo $background; ?>">
  <?php if($image) {?>
  <div class="layer <?php print $color; ?>_1">
  </div>
  <?php } else { ?>
  <div class="layer layer-opacity <?php print $color; ?>_1">
  </div>
  <?php }?>
  <div class="container">
    <div class="wrapper">
      <div class="content-main">
        <h1><?php print RoiMisc::truncate($headline, 60, TRUE);?></h1>
        <div class="row">
          <div class="col-md-6">
            <p class="description"><?php print RoiMisc::truncate($short_description, 360, TRUE);?></p>
          </div>
          <div class="col-md-4">
            <?php if(!empty($button_label) && !empty($link)): ?>
                <a class="btn btn-default btn-no-bg" href="<?php print $link; ?>" rel="nofollow" target="_blank"><?php print RoiMisc::truncate($button_label, 30, TRUE);?></a>
            <?php endif; ?>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>

