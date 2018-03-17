/*******************************<GINKGO LICENSE>******************************
Copyright 2017-2018

Karlsruhe Institute of Technology
Universitat Jaume I
University of Tennessee

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************<GINKGO LICENSE>*******************************/

#include <core/solver/cgs.hpp>


#include <gtest/gtest.h>


#include <random>


#include <core/base/exception.hpp>
#include <core/base/executor.hpp>
#include <core/matrix/dense.hpp>
#include <core/solver/cgs_kernels.hpp>
#include <core/test/utils.hpp>

namespace {

/*
class Cgs : public ::testing::Test {
protected:
    using Mtx = gko::matrix::Dense<>;
    Cgs() : rand_engine(30) {}

    void SetUp()
    {
        ASSERT_GT(gko::GpuExecutor::get_num_devices(), 0);
        ref = gko::ReferenceExecutor::create();
        gpu = gko::GpuExecutor::create(0, ref);
    }

    void TearDown()
    {
        if (gpu != nullptr) {
            ASSERT_NO_THROW(gpu->synchronize());
        }
    }

    std::unique_ptr<Mtx> gen_mtx(int num_rows, int num_cols)
    {
        return gko::test::generate_random_matrix<Mtx>(
            ref, num_rows, num_cols,
            std::uniform_int_distribution<>(num_cols, num_cols),
            std::normal_distribution<>(-1.0, 1.0), rand_engine);
    }

    void initialize_data()
    {
        int m = 97;
        int n = 43;
        b = gen_mtx(m, n);
        r = gen_mtx(m, n);
        r_tld = gen_mtx(m, n);
        t = gen_mtx(m, n);
        p = gen_mtx(m, n);
        q = gen_mtx(m, n);
        x = gen_mtx(m, n);
        u = gen_mtx(m, n);
        u_hat = gen_mtx(m, n);
        v_hat = gen_mtx(m, n);
        beta = gen_mtx(1, n);
        alpha = gen_mtx(1, n);
        gamma = gen_mtx(1, n);
        rho_prev = gen_mtx(1, n);
        rho = gen_mtx(1, n);

        d_b = Mtx::create(gpu);
        d_b->copy_from(b.get());
        d_r = Mtx::create(gpu);
        d_r->copy_from(r.get());
        d_r_tld = Mtx::create(gpu);
        d_r_tld->copy_from(r_tld.get());
        d_t = Mtx::create(gpu);
        d_t->copy_from(t.get());
        d_p = Mtx::create(gpu);
        d_p->copy_from(p.get());
        d_q = Mtx::create(gpu);
        d_q->copy_from(q.get());
        d_x = Mtx::create(gpu);
        d_x->copy_from(x.get());
        d_u = Mtx::create(gpu);
        d_u->copy_from(u.get());
        d_u_hat = Mtx::create(gpu);
        d_u_hat->copy_from(u_hat.get());
        d_v_hat = Mtx::create(gpu);
        d_v_hat->copy_from(v_hat.get());
        d_beta = Mtx::create(gpu);
        d_beta->copy_from(beta.get());
        d_alpha = Mtx::create(gpu);
        d_alpha->copy_from(alpha.get());
        d_gamma = Mtx::create(gpu);
        d_gamma->copy_from(gamma.get());
        d_rho_prev = Mtx::create(gpu);
        d_rho_prev->copy_from(rho_prev.get());
        d_rho = Mtx::create(gpu);
        d_rho->copy_from(rho.get());
    }

    void make_symetric(Mtx *mtx)
    {
        for (int i = 0; i < mtx->get_num_rows(); ++i) {
            for (int j = i + 1; j < mtx->get_num_cols(); ++j) {
                mtx->at(i, j) = mtx->at(j, i);
            }
        }
    }

    void make_diag_dominant(Mtx *mtx)
    {
        using std::abs;
        for (int i = 0; i < mtx->get_num_rows(); ++i) {
            auto sum = gko::zero<Mtx::value_type>();
            for (int j = 0; j < mtx->get_num_cols(); ++j) {
                sum += abs(mtx->at(i, j));
            }
            mtx->at(i, i) = sum;
        }
    }

    void make_spd(Mtx *mtx)
    {
        make_symetric(mtx);
        make_diag_dominant(mtx);
    }

    std::shared_ptr<gko::ReferenceExecutor> ref;
    std::shared_ptr<const gko::GpuExecutor> gpu;

    std::ranlux48 rand_engine;

    std::unique_ptr<Mtx> b;
    std::unique_ptr<Mtx> r;
    std::unique_ptr<Mtx> r_tld;
    std::unique_ptr<Mtx> t;
    std::unique_ptr<Mtx> p;
    std::unique_ptr<Mtx> q;
    std::unique_ptr<Mtx> u;
    std::unique_ptr<Mtx> u_hat;
    std::unique_ptr<Mtx> v_hat;
    std::unique_ptr<Mtx> x;
    std::unique_ptr<Mtx> beta;
    std::unique_ptr<Mtx> alpha;
    std::unique_ptr<Mtx> gamma;
    std::unique_ptr<Mtx> rho_prev;
    std::unique_ptr<Mtx> rho;

    std::unique_ptr<Mtx> d_b;
    std::unique_ptr<Mtx> d_r;
    std::unique_ptr<Mtx> d_r_tld;
    std::unique_ptr<Mtx> d_t;
    std::unique_ptr<Mtx> d_p;
    std::unique_ptr<Mtx> d_q;
    std::unique_ptr<Mtx> d_u;
    std::unique_ptr<Mtx> d_u_hat;
    std::unique_ptr<Mtx> d_v_hat;
    std::unique_ptr<Mtx> d_x;
    std::unique_ptr<Mtx> d_beta;
    std::unique_ptr<Mtx> d_alpha;
    std::unique_ptr<Mtx> d_gamma;
    std::unique_ptr<Mtx> d_rho_prev;
    std::unique_ptr<Mtx> d_rho;
};


TEST_F(Cgs, GpuCgsInitializeIsEquivalentToRef)
{
    initialize_data();

    gko::kernels::reference::cgs::initialize(
        ref, b.get(), r.get(), r_tld.get(), p.get(), q.get(), u.get(),
        u_hat.get(), v_hat.get(), t.get(), alpha.get(), beta.get(), gamma.get(),
        rho_prev.get(), rho.get());
    gko::kernels::gpu::cgs::initialize(
        gpu, d_b.get(), d_r.get(), d_r_tld.get(), d_p.get(), d_q.get(),
        d_u.get(), d_u_hat.get(), d_v_hat.get(), d_t.get(), d_alpha.get(),
        d_beta.get(), d_gamma.get(), d_rho_prev.get(), d_rho.get());

    ASSERT_MTX_NEAR(d_r, r, 1e-14);
    ASSERT_MTX_NEAR(d_r_tld, r_tld, 1e-14);
    ASSERT_MTX_NEAR(d_p, p, 1e-14);
    ASSERT_MTX_NEAR(d_q, q, 1e-14);
    ASSERT_MTX_NEAR(d_u, u, 1e-14);
    ASSERT_MTX_NEAR(d_u_hat, u_hat, 1e-14);
    ASSERT_MTX_NEAR(d_v_hat, v_hat, 1e-14);
    ASSERT_MTX_NEAR(d_t, t, 1e-14);
    ASSERT_MTX_NEAR(d_alpha, alpha, 1e-14);
    ASSERT_MTX_NEAR(d_beta, beta, 1e-14);
    ASSERT_MTX_NEAR(d_gamma, gamma, 1e-14);
    ASSERT_MTX_NEAR(d_rho_prev, rho_prev, 1e-14);
    ASSERT_MTX_NEAR(d_rho, rho, 1e-14);
}


TEST_F(Cgs, GpuCgsStep1IsEquivalentToRef)
{
    initialize_data();

    gko::kernels::reference::cgs::step_1(ref, r.get(), u.get(), p.get());
    gko::kernels::gpu::cgs::step_1(gpu, d_r.get(), d_u.get(), d_p.get());

    ASSERT_MTX_NEAR(d_u, u, 1e-14);
    ASSERT_MTX_NEAR(d_p, p, 1e-14);
}

TEST_F(Cgs, GpuCgsStep2IsEquivalentToRef)
{
    initialize_data();

    gko::kernels::reference::cgs::step_2(ref, r.get(), u.get(), p.get(),
                                         q.get(), beta.get(), rho.get(),
                                         rho_prev.get());
    gko::kernels::gpu::cgs::step_2(gpu, d_r.get(), d_u.get(), d_p.get(),
                                   d_q.get(), d_beta.get(), d_rho.get(),
                                   d_rho_prev.get());
    ASSERT_MTX_NEAR(d_beta, beta, 1e-14);
    ASSERT_MTX_NEAR(d_u, u, 1e-14);
    ASSERT_MTX_NEAR(d_p, p, 1e-14);
}


TEST_F(Cgs, GpuCgsStep3IsEquivalentToRef)
{
    initialize_data();
    gko::kernels::reference::cgs::step_3(ref, u.get(), v_hat.get(), q.get(),
                                         t.get(), alpha.get(), rho.get(),
                                         gamma.get());
    gko::kernels::gpu::cgs::step_3(gpu, d_u.get(), d_v_hat.get(), d_q.get(),
                                   d_t.get(), d_alpha.get(), d_rho.get(),
                                   d_gamma.get());

    ASSERT_MTX_NEAR(d_alpha, alpha, 1e-14);
    ASSERT_MTX_NEAR(d_q, q, 1e-14);
    ASSERT_MTX_NEAR(d_t, t, 1e-14);
}

TEST_F(Cgs, GpuCgsStep4IsEquivalentToRef)
{
    initialize_data();
    gko::kernels::reference::cgs::step_4(ref, t.get(), u_hat.get(), r.get(),
                                         x.get(), alpha.get());
    gko::kernels::gpu::cgs::step_4(gpu, d_t.get(), d_u_hat.get(), d_r.get(),
                                   d_x.get(), d_alpha.get());
    ASSERT_MTX_NEAR(d_r, r, 1e-14);
    ASSERT_MTX_NEAR(d_x, x, 1e-14);
}


TEST_F(Cgs, ApplyIsEquivalentToRef)
{
    auto mtx = gen_mtx(50, 50);
    make_spd(mtx.get());
    auto x = gen_mtx(50, 3);
    auto b = gen_mtx(50, 3);
    auto d_mtx = Mtx::create(gpu);
    d_mtx->copy_from(mtx.get());
    auto d_x = Mtx::create(gpu);
    d_x->copy_from(x.get());
    auto d_b = Mtx::create(gpu);
    d_b->copy_from(b.get());
    auto cgs_factory = gko::solver::CgsFactory<>::create(ref, 50, 1e-14);
    auto d_cgs_factory = gko::solver::CgsFactory<>::create(gpu, 50, 1e-14);
    auto solver = cgs_factory->generate(std::move(mtx));
    auto d_solver = d_cgs_factory->generate(std::move(d_mtx));

    solver->apply(b.get(), x.get());
    d_solver->apply(d_b.get(), d_x.get());

    ASSERT_MTX_NEAR(d_x, x, 1e-14);
}
*/

}  // namespace
