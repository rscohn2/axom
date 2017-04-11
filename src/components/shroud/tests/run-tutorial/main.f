!
! Test code generated by the tutorial test
!
program tester
  use fruit
  use iso_c_binding
  use tutorial_mod
  implicit none
  logical ok

  logical rv_logical
  integer rv_integer
  integer(C_INT) rv_int
  real(C_DOUBLE) rv_double
  character(30) rv_char

  character(4)  status

  call init_fruit

  call test_functions

  call test_class1

  call fruit_summary
  call fruit_finalize

  call is_all_successful(ok)
  if (.not. ok) then
     call exit(1)
  endif

contains

  subroutine test_functions

    call set_case_name("test_functions")

    call function1
    call assert_true(.true.)

    rv_double = function2(1.d0, 4)
    call assert_true(rv_double == 5.d0)

    rv_logical = function3(.false.)
    call assert_true(rv_logical)

    rv_logical = .true.
    call function3b(.true., rv_logical)
    call assert_false(rv_logical)

    rv_logical = .false.
    call function3b(.false., rv_logical)
    call assert_true(rv_logical)

    call assert_true( function4a("dog", "cat") == "dogcat")

    call function4b("dog", "cat", rv_char)
    call assert_true( rv_char == "dogcat")

    call assert_equals(function5(), 13.1415d0)
    call assert_equals(function5(1.d0), 11.d0)
    call assert_equals(function5(1.d0, .false.), 1.d0)

    call function6("name")
    call assert_true(last_function_called() == "Function6(string)")
    call function6(1)
    call assert_true(last_function_called() == "Function6(int)")

    call function7(1)
    call assert_true(last_function_called() == "Function7<int>")
    call function7(10.d0)
    call assert_true(last_function_called() == "Function7<double>")

    ! return values set by calls to function7
    rv_integer = function8_int()
    call assert_true(rv_integer == 1)
    rv_double = function8_double()
    call assert_true(rv_double == 10.d0)

    call function9(1.0)
    call assert_true(.true.)
    call function9(1.d0)
    call assert_true(.true.)

    call function10()
    call assert_true(.true.)
    call function10("foo", 1.0e0)
    call assert_true(.true.)
    call function10("bar", 2.0d0)
    call assert_true(.true.)

    call sum(5, [1,2,3,4,5], rv_int)
    call assert_true(rv_int .eq. 15)

    rv_int = overload1(10)
    call assert_true(rv_int .eq. 10)
    rv_int = overload1(1.0d0, 10)
    call assert_true(rv_int .eq. 10)

    rv_int = overload1(10, 11, 12)
    call assert_true(rv_int .eq. 142)
    rv_int = overload1(1.0d0, 10, 11, 12)
    call assert_true(rv_int .eq. 142)

    rv_int = typefunc(2)
    call assert_true(rv_int .eq. 2)

    rv_int = enumfunc(1)
    call assert_true(rv_int .eq. 2)

  end subroutine test_functions

  subroutine test_class1
    type(class1) obj

    call set_case_name("test_class1")

    obj = class1_new()
    call assert_true(c_associated(obj%get_instance()), "class1_new")

    call obj%method1
    call assert_true(.true.)

    call useclass(obj)

    call obj%delete
    call assert_true(.not. c_associated(obj%get_instance()), "class1_delete")
  end subroutine test_class1

end program tester
