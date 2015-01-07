
#include "tools/gstswitchopts.c"

static void
test_strings_good (void)
{
  g_assert_cmpint (parse_format ("debug"), ==, 0);
  g_assert_cmpint (parse_format ("pal"), ==, 0);
  g_assert_cmpint (parse_format ("720p60"), ==, 0);
  g_assert_cmpint (parse_format ("1024x768@60"), ==, 0);
  g_assert_cmpint (parse_format ("VGA@60"), ==, 0);
  g_assert_cmpint (parse_format ("4k@60"), ==, 0);
  g_assert_cmpint (parse_format
      ("video/x-raw,height=400,width=500,framerate=25/1"), ==, 0);
}

static void
test_strings_bad (void)
{
  g_assert_cmpint (parse_format
      ("video/x-raw,height=[400,800],width=500,framerate=25/1"), ==, -1);
  g_assert_cmpint (parse_format ("720p@75"), ==, -1);
  g_assert_cmpint (parse_format ("bad-format-string"), ==, -1);
  g_assert_cmpint (parse_format
      ("video/x-raw,height=10,width=500,framerate=25/1"), ==, -1);
  g_assert_cmpint (parse_format
      ("video/x-raw,height=400,width=10,framerate=25/1"), ==, -1);
  g_assert_cmpint (parse_format
      ("video/x-raw,height=400,width=10,framerate=1001/1"), ==, -1);
// the following actually works but it shouldn't
//  g_assert_cmpint (parse_format("pal@75"), ==, -1);
}

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);
  gst_init (&argc, &argv);
  g_test_set_nonfatal_assertions ();
  g_test_add_func ("/gstswitch/options/format/good", test_strings_good);
  g_test_add_func ("/gstswitch/options/format/bad", test_strings_bad);
  return g_test_run ();
}
