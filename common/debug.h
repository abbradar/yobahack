#ifndef YOBAHACK_COMMON_DEBUG_H_
#define YOBAHACK_COMMON_DEBUG_H_

#include <sstream>
#include "common/logging.h"
#include "common/application.h"

#ifndef DEBUG_LEVEL
/** Enables various debugging code. It is a number between 0 and 4.
 * 0: All assertions and debug code are disabled.
 * 1: Basic assertions and sanity checks.
 * 2: Various checks that do not slow down application.
 * 3: Checks that can slow down application, but provide useful information and checks.
 * 4: Very verbose checks, speed does not matter. (used only during debugging)
 */
#define DEBUG_LEVEL 1
#endif

#if DEBUG_LEVEL == 0
#define BOOST_DISABLE_ASSERTS
#define NDEBUG
#else
#define BOOST_ENABLE_ASSERT_HANDLER
#endif

#include <boost/assert.hpp>

/** Similar to standard assert(), but sends error to logging subsystem. */
#define Assert BOOST_ASSERT
/** Similar to ASSERT, but adds message to error message.
 * \sa Assert
 */
#define AssertMsg BOOST_ASSERT_MSG

#endif // YOBAHACK_COMMON_DEBUG_H_
