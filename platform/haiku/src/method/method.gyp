{
  'variables': {
    'relative_dir': 'src/method',
    'gen_out_dir': '<(SHARED_INTERMEDIATE_DIR)/<(relative_dir)',
    'out_dir': '<(PRODUCT_DIR)',
    'backend_dir': '../../../../src',
    'bridge_dir': '../../../../src/bridge',
    'engine_dir': '../../../../src/engine',
    'method_pre_path': '<(gen_out_dir)/HaikuMethod.pre',
    'method_rsrc_path': '<(gen_out_dir)/HaikuMethod.rsrc',
    'app_mime_sig': 'x-vnd.AquaSKK-InputMethod',
  },
  'targets': [
    {
      'target_name': 'aquaskk',
      'type': 'none',
      'dependencies': [
        'aquaskk_no_resource',
        'method_rsrc',
        #'catalogs',
      ],
      'actions': [
        {
          'action_name': 'merge_rsrc',
          'inputs': [
            '<(out_dir)/aquaskk_no_resource',
          ],
          'outputs': [
            '<(out_dir)/aquaskk',
          ],
          'action': [
            'python', '../tools/xres.py',
            '--outpath', '<@(_outputs)',
            '--inputpath', '<@(_inputs)',
            '--rsrc', '<(method_rsrc_path)',
          ],
          'message': 'Generating <@(_outputs)',
        },
      ],
    },
    {
      'target_name': 'aquaskk_no_resource',
      'type': 'executable',
      'conditions': [
        ['arch!="x86_gcc2"', {
          'dependencies': [
            'engine_lib',
            'method_lib',
          ],
          'include_dirs': [
            './',
          ],
          'link_settings': {
            'libraries': [
              '-lbe',
              '-ltranslation',
              '-llocalestub',
              '-lshared',
              '/boot/system/servers/input_server',
            ],
          },
          'sources': [
            'HaikuMethod.cpp',
          ],
        }, {
          # arch=="x86_gcc2"
          'dependencies': [
            'aquaskk_task',
          ],
          'actions': [
            {
              'action_name': 'aquaskk_no_resource_x86_gcc2',
              'inputs': [
                'HaikuMethod.cpp',
              ],
              'outputs': [
                '<(out_dir)/aquaskk_no_resource',
              ],
              'action': [
                'python', '../tools/compile.py',
                '--compiler', '/bin/g++',
                '--inputpath', '<@(_inputs)',
                '--outpath', '<@(_outputs)',
                '--defs', 'X86_GCC2',
                '--includes', './', '../..',
                '--libs', 'be', '/boot/system/servers/input_server',
              ],
              'message': 'Generating <@(_outputs)',
            },
          ],
        },]
      ],
    },
    {
      'target_name': 'method_lib',
      'type': 'static_library',
      'include_dirs': [
        './',
        '<(backend_dir)/bridge',
        '<(backend_dir)/engine/backend',
        '<(backend_dir)/engine/completer',
        '<(backend_dir)/engine/dictionary',
        '<(backend_dir)/engine/editor',
        '<(backend_dir)/engine/entry',
        '<(backend_dir)/engine/keymap',
        '<(backend_dir)/engine/selector',
        '<(backend_dir)/engine/session',
        '<(backend_dir)/engine/skkserv',
        '<(backend_dir)/engine/state',
        '<(backend_dir)/engine/trie',
        '<(backend_dir)/engine/utility',
        '../gui',
      ],
      'sources': [
        'HaikuAnnotator.cpp',
        'HaikuCandidateWindow.cpp',
        'HaikuClipboard.cpp',
        'HaikuConfig.cpp',
        'HaikuDynamicCompletor.cpp',
        'HaikuFrontEnd.cpp',
        'HaikuInputSessionParameter.cpp',
        'HaikuInputSessionParameter.cpp',
        'HaikuLooper.cpp',
        'HaikuMessenger.cpp',
        'HaikuModeIndicator.cpp',
        'SKKConstVars.cpp',
        'SKKPreProcessor.cpp',
        '../gui/CandidateView.cpp',
        '../gui/CandidateWindow.cpp',
        '../gui/CompletionView.cpp',
        '../gui/CompletionWindow.cpp',
        '../gui/InputModeWindow.cpp',
        '../gui/MessageWindow.cpp',
      ],
      'conditions': [
        ['arch=="x86_gcc2"', {
          'defines': [
            'X86_GCC2',
          ],
        },],
      ],
    },
    {
      'target_name': 'engine_lib',
      'type': 'static_library',
      'include_dirs': [
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
      ],
      'sources': [
        '<(engine_dir)/backend/SKKBackEnd.cpp',
        '<(engine_dir)/backend/SKKDictionaryCache.cpp',
        '<(engine_dir)/backend/SKKDictionaryFactory.cpp',
        '<(engine_dir)/backend/SKKNumericConverter.cpp',
        '<(engine_dir)/completer/SKKCompleter.cpp',
        '<(engine_dir)/dictionary/SKKDictionaryFile.cpp',
        '<(engine_dir)/dictionary/SKKDictionaryKeeper.cpp',
        '<(engine_dir)/dictionary/SKKDistributedUserDictionary.cpp',
        '<(engine_dir)/dictionary/SKKGadgetDictionary.cpp',
        '<(engine_dir)/dictionary/SKKHttpDictionaryLoader.cpp',
        '<(engine_dir)/dictionary/SKKLocalUserDictionary.cpp',
        '<(engine_dir)/dictionary/SKKProxyDictionary.cpp',
        '<(engine_dir)/editor/SKKCandidateEditor.cpp',
        '<(engine_dir)/editor/SKKComposingEditor.cpp',
        '<(engine_dir)/editor/SKKEntryRemoveEditor.cpp',
        '<(engine_dir)/editor/SKKInputEngine.cpp',
        '<(engine_dir)/editor/SKKInputQueue.cpp',
        '<(engine_dir)/editor/SKKOkuriEditor.cpp',
        '<(engine_dir)/editor/SKKPrimaryEditor.cpp',
        '<(engine_dir)/editor/SKKRegisterEditor.cpp',
        '<(engine_dir)/editor/SKKTextBuffer.cpp',
        '<(engine_dir)/entry/SKKCandidate.cpp',
        '<(engine_dir)/entry/SKKEntry.cpp',
        '<(engine_dir)/keymap/SKKKeymap.cpp',
        '<(engine_dir)/keymap/SKKKeymapEntry.cpp',
        '<(engine_dir)/selector/SKKInlineSelector.cpp',
        '<(engine_dir)/selector/SKKSelector.cpp',
        '<(engine_dir)/selector/SKKWindowSelector.cpp',
        '<(engine_dir)/session/SKKInputEnvironment.cpp',
        '<(engine_dir)/session/SKKInputModeSelector.cpp',
        '<(engine_dir)/session/SKKInputSession.cpp',
        '<(engine_dir)/session/SKKOutputBuffer.cpp',
        '<(engine_dir)/session/SKKRecursiveEditor.cpp',
        '<(engine_dir)/session/SKKUndoContext.cpp',
        '<(engine_dir)/skkserv/skkserv.cpp',
        '<(engine_dir)/state/SKKState.cpp',
        '<(engine_dir)/trie/SKKRomanKanaConverter.cpp',
        '<(engine_dir)/trie/SKKTrie.cpp',
        '<(engine_dir)/utility/jconv.cpp',
      ],
    },
    {
      'target_name': 'method_rsrc',
      'type': 'none',
      'actions': [
        {
          'action_name': 'rdef_to_rsrc',
          'inputs': [
            './HaikuMethod.rdef',
          ],
          'outputs': [
            '<(method_rsrc_path)',
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
      'target_name': 'method_pre',
      'type': 'none',
      'actions': [
        {
          'action_name': 'generate_pre',
          'inputs': [
            'HaikuLooper.cpp',
          ],
          'outputs': [
            '<(method_pre_path)',
          ],
          'inc': [
            './',
            '../gui',
            '<(bridge_dir)',
            '<(engine_dir)/backend',
            '<(engine_dir)/completer',
            '<(engine_dir)/dictionary',
            '<(engine_dir)/editor',
            '<(engine_dir)/entry',
            '<(engine_dir)/keymap',
            '<(engine_dir)/selector',
            '<(engine_dir)/session',
            '<(engine_dir)/skkserv',
            '<(engine_dir)/state',
            '<(engine_dir)/trie',
            '<(engine_dir)/utility',
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
        'method_pre',
      ],
      'actions': [
        {
          'action_name': 'generate_catkeys',
          'inputs': [
            '<(method_pre_path)',
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
  'conditions': [
    ['arch=="x86_gcc2"', {
      'targets': [
        {
          'target_name': 'aquaskk_task',
          'type': 'none',
          'dependencies': [
            'aquaskk_task_no_resource',
            'aquaskk_task_rsrc',
          ],
          'actions': [
            {
              'action_name': 'task_merge_rsrc',
              'inputs': [
                '<(out_dir)/aquaskk_task_no_resource',
              ],
              'outputs': [
                '<(out_dir)/aquaskk_task',
              ],
              'action': [
                'python', '../tools/xres.py',
                '--outpath', '<@(_outputs)',
                '--inputpath', '<@(_inputs)',
                '--rsrc', '<(gen_out_dir)/Task.rsrc',
                '--exe',
              ],
            },
          ],
        },
        {
          'target_name': 'aquaskk_task_no_resource',
          'type': 'executable',
          'dependencies': [
            'method_lib',
            'engine_lib',
          ],
          'sources': [
            'Task.cpp',
          ],
          'link_settings': {
            'libraries': [
              '-lbe',
              '-ltranslation',
              '-llocalestub',
              '-lshared',
            ],
          },
          'defines': [
            'X86_GCC2',
          ],
        },
        {
          'target_name': 'aquaskk_task_rsrc',
          'type': 'none',
          'actions': [
            {
              'action_name': 'aquaskk_task_rdef_to_rsrc',
              'inputs': [
                'Task.rdef',
              ],
              'outputs': [
                '<(gen_out_dir)/Task.rsrc',
              ],
              'action': [
                'python', '../tools/rc.py',
                '--outpath', '<@(_outputs)',
                '<@(_inputs)',
              ],
            }
          ],
        },
      ],
    },]
  ],
}
