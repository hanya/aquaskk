
{
  'variables': {
  }, # variables
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'defines': ['DEBUG'],
        'cflags': ['-g', '-O0'],
      },
      'Release': {
        'cflags': ['-O3',],
      },
    },
    'conditions': [
      ['platform=="Haiku"', {
        'cflags': [
          '-fPIC',
          '-fno-exceptions',
        ],
        'cflags_cc': [
          '-std=c++11',
          '-fexceptions',
          #'-Wall',
          #'-Wextra',
          #'-Werror',
          #'-Werror=deprecated-declarations',
          #'-Werror=unused-parameter',
        ],
        'defines': [
          'OS_HAIKU',
        ],
        'link_settings': {
          'libraries': [
            '-lroot',
            '-lnetwork',
          ],
        },
      }],
    ], # conditions
  }, # target_defaults
}
