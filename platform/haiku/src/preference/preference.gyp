{
  'variables': {
    'relative_dir': 'src/preference',
    'gen_out_dir': '<(SHARED_INTERMEDIATE_DIR)/<(relative_dir)',
    'out_dir': '<(PRODUCT_DIR)',
    'backend_dir': '../../../../src',
    'bridge_dir': '../../../../src/bridge',
    'engine_dir': '../../../../src/engine',
    'preference_pre_path': '<(gen_out_dir)/Preference.pre',
    'preference_rsrc_path': '<(gen_out_dir)/Preference.rsrc',
    'app_mime_sig': 'x-vnd.AquaSKK-Preference',
  },
  'targets': [
    {
      'target_name': 'preference',
      'type': 'none',
      'dependencies': [
        'preference_no_resource',
        'preference_rsrc',
        #'catalogs',
      ],
      'actions': [
        {
          'action_name': 'merge_rsrc',
          'inputs': [
            '<(out_dir)/preference_no_resource',
          ],
          'outputs': [
            '<(out_dir)/aquaskk_preference',
          ],
          'action': [
            'python', '../tools/xres.py',
            '--outpath', '<@(_outputs)',
            '--inputpath', '<@(_inputs)',
            '--rsrc', '<(preference_rsrc_path)',
            '--exe'
          ],
          'message': 'Generating <@(_outputs)',
        },
      ],
    },
    {
      'target_name': 'preference_no_resource',
      'type': 'executable',
      'dependencies': [
        'preference_lib',
      ],
      'include_dirs': [
        './',
        '../method',
      ],
      'link_settings': {
        'libraries': [
          '-lbe',
          '-llocalestub',
          '-lshared',
          '-ltracker',
        ],
      },
      'sources': [
        'PreferenceApp.cpp',
      ],
    },
    {
      'target_name': 'preference_lib',
      'type': 'static_library',
      'dependencies': [
        'version_h',
      ],
      'include_dirs': [
        './',
        '../method',
        '<(out_dir)',
        '/boot/system/develop/headers/private/interface',
      ],
      'sources': [
        'CompletionView.cpp',
        'ConversionView.cpp',
        'DictionaryView.cpp',
        'DisplayView.cpp',
        'ExtensionView.cpp',
        'FileChooser.cpp',
        'FileFilter.cpp',
        'LocGridView.cpp',
        'InputView.cpp',
        'MiscView.cpp',
        'PreferenceDialog.cpp',
        'VersionView.cpp',
      ],
    },
    {
      'target_name': 'version_h',
      'type': 'none',
      'actions': [
        {
          'action_name': 'gen_version',
          'inputs': [
            '../../../mac/proj/AquaSKKPreferences-Info.plist',
          ],
          'outputs': [
            '<(out_dir)/version.h',
          ],
          'action': [
            'python', '../tools/gen_version.py',
            '--outpath', '<@(_outputs)',
            '--inputpath', '<@(_inputs)',
          ],
        },
      ],
    },
    {
      'target_name': 'preference_rsrc',
      'type': 'none',
      'actions': [
        {
          'action_name': 'rdef_to_rsrc',
          'inputs': [
            './PreferenceApp.rdef',
          ],
          'outputs': [
            '<(preference_rsrc_path)',
          ],
          'action': [
            'python', '../tools/rc.py',
            '--outpath', '<@(_outputs)',
            '<@(_inputs)',
          ],
        },
      ],
    },
    {
      'target_name': 'preference_pre',
      'type': 'none',
      'actions': [
        {
          'action_name': 'generate_pre',
          'inputs': [
            'CompletionView.cpp',
            'ConversionView.cpp',
            'DictionaryView.cpp',
            'DisplayView.cpp',
            'ExtensionView.cpp',
            'FileChooser.cpp',
            'FileFilter.cpp',
            'InputView.cpp',
            'MiscView.cpp',
            'PreferenceDialog.cpp',
            'VersionView.cpp',
          ],
          'outputs': [
            '<(preference_pre_path)',
          ],
          'inc': [
            './',
            '../method',
            '<(bridge_dir)',
            '<(engine_dir)/backend',
            '<(engine_dir)/completer',
            '<(engine_dir)/dictionary',
            '<(engine_dir)/editor',
            '<(engine_dir)/entry',
            '<(engine_dir)/keymap',
            '<(engine_dir)/selector',
            '<(engine_dir)/session',
            '<(engine_dir)/state',
            '<(engine_dir)/trie',
            '<(engine_dir)/utility',
            '/boot/system/develop/headers/private/interface',
          ],
          'action': [
            'python', '../tools/gen_pre.py',
            '--parser', 'gcc',
            '--outpath', '<@(_outputs)',
            '--includes', '<@(_inc)',
            '--inputpath', '<@(_inputs)',
          ],
        },
      ],
    },
    {
      'target_name': 'catalogs',
      'type': 'none',
      'actions': [
        {
          'action_name': 'convert_catalogs',
          'inputs': [
            './locales/en.catkeys',
            './locales/ja.catkeys',
          ],
          'outputs': [
            '<(out_dir)/<@(app_mime_sig)',
          ],
          'action': [
            'python', '../tools/gen_catalogs.py',
            '--sig', '<@(app_mime_sig)',
            '--outpath', '<@(_outputs)',
            '--inputpath', '<@(_inputs)',
          ],
        },
      ],
    },
    {
      'target_name': 'catkeys',
      'type': 'none',
      'dependencies': [
        'preference_pre',
      ],
      'actions': [
        {
          'action_name': 'generate_catkeys',
          'inputs': [
            '<(preference_pre_path)',
          ],
          'outputs': [
            './locales/en.catkeys',
          ],
          'action': [
            'python', '../tools/gen_catkeys.py',
            '--sig', '<@(app_mime_sig)',
            '--outpath', '<@(_outputs)',
            '--inputpath', '<@(_inputs)',
          ],
        },
      ],
    },
  ],
}
